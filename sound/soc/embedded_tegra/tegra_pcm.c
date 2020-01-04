/*
 * tegra_pcm.c  --  ALSA Soc Audio Layer
 *
 * (c) 2010 Nvidia Graphics Pvt. Ltd.
 *  http://www.nvidia.com
 *
 * (c) 2006 Wolfson Microelectronics PLC.
 * Graeme Gregory graeme.gregory@wolfsonmicro.com or linux@wolfsonmicro.com
 *
 * (c) 2004-2005 Simtec Electronics
 *    http://armlinux.simtec.co.uk/
 *    Ben Dooks <ben@simtec.co.uk>
 *
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the  License, or (at your
 *  option) any later version.
 */

#include "tegra_soc.h"
#include <mach/tegra_das.h>

#define PLAYBACK_STARTED true
#define PLAYBACK_STOPPED false

int setup_dma_request(struct snd_pcm_substream *substream,
		      struct tegra_dma_req *req,
		      void (*dma_callback) (struct tegra_dma_req * req),
		      void *dma_data)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_dai *cpu_dai = rtd->dai->cpu_dai;
	struct tegra_i2s_info *info = cpu_dai->private_data;

	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
		req->to_memory = false;
		req->dest_addr =
		    i2s_get_fifo_phy_base(cpu_dai->id, I2S_FIFO_TX);
		req->dest_wrap = 4;
		req->source_wrap = 0;
	} else {
		req->to_memory = true;
		req->source_addr =
		    i2s_get_fifo_phy_base(cpu_dai->id, I2S_FIFO_RX);
		req->dest_wrap = 0;
		req->source_wrap = 4;
	}

	req->complete = dma_callback;
	req->dev = dma_data;
	req->source_bus_width = 32;
	req->dest_bus_width = 32;
	req->req_sel = info->dma_req_sel;

	return 0;
}

static void tegra_pcm_play(struct tegra_runtime_data *prtd)
{
	struct snd_pcm_substream *substream = prtd->substream;
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct snd_dma_buffer *buf = &substream->dma_buffer;

	if (runtime->dma_addr) {
		prtd->size = frames_to_bytes(runtime, runtime->period_size);
		if (prtd->dma_state != STATE_ABORT) {
			prtd->dma_reqid_tail =
			    (prtd->dma_reqid_tail + 1) % DMA_REQ_QCOUNT;
			prtd->dma_req[prtd->dma_reqid_tail].source_addr =
			    buf->addr + frames_to_bytes(runtime, prtd->dma_pos);
			prtd->dma_req[prtd->dma_reqid_tail].size = prtd->size;
			tegra_dma_enqueue_req(prtd->dma_chan,
					      &prtd->dma_req[prtd->
							     dma_reqid_tail]);
		}
	}

	prtd->dma_pos += runtime->period_size;
	if (prtd->dma_pos >= runtime->buffer_size) {
		prtd->dma_pos = 0;
	}

}

static void tegra_pcm_capture(struct tegra_runtime_data *prtd)
{
	struct snd_pcm_substream *substream = prtd->substream;
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct snd_dma_buffer *buf = &substream->dma_buffer;

	if (runtime->dma_addr) {
		prtd->size = frames_to_bytes(runtime, runtime->period_size);
		if (prtd->dma_state != STATE_ABORT) {
			prtd->dma_reqid_tail =
			    (prtd->dma_reqid_tail + 1) % DMA_REQ_QCOUNT;
			prtd->dma_req[prtd->dma_reqid_tail].dest_addr =
			    buf->addr + frames_to_bytes(runtime, prtd->dma_pos);
			prtd->dma_req[prtd->dma_reqid_tail].size = prtd->size;
			tegra_dma_enqueue_req(prtd->dma_chan,
					      &prtd->dma_req[prtd->
							     dma_reqid_tail]);
		}

	}
	prtd->dma_pos += runtime->period_size;
	if (prtd->dma_pos >= runtime->buffer_size) {
		prtd->dma_pos = 0;
	}

}

static void dma_complete_callback(struct tegra_dma_req *req)
{
	struct tegra_runtime_data *prtd = (struct tegra_runtime_data *)req->dev;
	struct snd_pcm_substream *substream = prtd->substream;
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct tegra_i2s_info *info = prtd->dai->private_data;

	if (++prtd->period_index >= runtime->periods) {
		prtd->period_index = 0;
	}

	if (prtd->dma_state != STATE_ABORT) {
		prtd->dma_reqid_head =
		    (prtd->dma_reqid_head + 1) % DMA_REQ_QCOUNT;
		snd_pcm_period_elapsed(substream);
		if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
			if (info->fifo_err) {
				int i;

				info->fifo_err = 0;

				i2s_tdm_set_tdm_tx_disable(prtd->dai->id);
				tegra_dma_cancel(prtd->dma_chan);

				for (i = 0; i < DMA_REQ_QCOUNT; i++)
					tegra_dma_dequeue_req(prtd->dma_chan, &prtd->dma_req[i]);

				prtd->dma_reqid_head = 0;
				prtd->dma_reqid_tail = DMA_REQ_QCOUNT - 1;

				i2s_fifo_clear(prtd->dai->id, I2S_FIFO_TX);

				tegra_pcm_play(prtd);	/* dma enqueue req1 */
				tegra_pcm_play(prtd);	/* dma enqueue req2 */
				i2s_tdm_set_tdm_tx_en(prtd->dai->id);
				pr_info("I2S-%d: fifo underrun",prtd->dai->id);
			}
			else
				tegra_pcm_play(prtd);
		} else {
			tegra_pcm_capture(prtd);
		}
	}
}

static const struct snd_pcm_hardware tegra_pcm_hardware_i2s = {
	.info = SNDRV_PCM_INFO_INTERLEAVED |
	    SNDRV_PCM_INFO_PAUSE | SNDRV_PCM_INFO_RESUME |
	    SNDRV_PCM_INFO_MMAP | SNDRV_PCM_INFO_MMAP_VALID,
	.formats = SNDRV_PCM_FMTBIT_S16_LE,
	.channels_min = 2,
	.channels_max = 2,
	.buffer_bytes_max = (PAGE_SIZE * 8),
	.period_bytes_min = 1024,
	.period_bytes_max = (PAGE_SIZE),
	.periods_min = 4,
	.periods_max = 8,
	.fifo_size = 4,
};

static const struct snd_pcm_hardware tegra_pcm_hardware_tdm1 = {
	.info = SNDRV_PCM_INFO_INTERLEAVED |
	    SNDRV_PCM_INFO_PAUSE | SNDRV_PCM_INFO_RESUME |
	    SNDRV_PCM_INFO_MMAP | SNDRV_PCM_INFO_MMAP_VALID,
	.formats = SNDRV_PCM_FMTBIT_S16_LE,
	.channels_min = 8,
	.channels_max = 8,
	.buffer_bytes_max = (1024 * 4 * 8),
	.period_bytes_min = 1024 * 8,
	.period_bytes_max = 1024 * 8,
	.periods_min = 4,
	.periods_max = 4,
	.fifo_size = 4,
};

static const struct snd_pcm_hardware tegra_pcm_hardware_tdm2 = {
	.info = SNDRV_PCM_INFO_INTERLEAVED |
	    SNDRV_PCM_INFO_PAUSE | SNDRV_PCM_INFO_RESUME |
	    SNDRV_PCM_INFO_MMAP | SNDRV_PCM_INFO_MMAP_VALID,
	.formats = SNDRV_PCM_FMTBIT_S16_LE,
	.channels_min = 16,
	.channels_max = 16,
	.buffer_bytes_max = (1024 * 4 * 16),
	.period_bytes_min = 1024 * 16,
	.period_bytes_max = 1024 * 16,
	.periods_min = 4,
	.periods_max = 4,
	.fifo_size = 4,
};

static int tegra_pcm_hw_params(struct snd_pcm_substream *substream,
			       struct snd_pcm_hw_params *params)
{
	snd_pcm_set_runtime_buffer(substream, &substream->dma_buffer);
	return 0;
}

static int tegra_pcm_hw_free(struct snd_pcm_substream *substream)
{
	snd_pcm_set_runtime_buffer(substream, NULL);
	return 0;
}

static int tegra_pcm_prepare(struct snd_pcm_substream *substream)
{
	struct tegra_runtime_data *prtd = substream->runtime->private_data;

	prtd->dma_pos = 0;
	prtd->period_index = 0;
	prtd->dma_reqid_head = 0;
	prtd->dma_reqid_tail = DMA_REQ_QCOUNT - 1;

	return 0;
}

static int tegra_pcm_trigger(struct snd_pcm_substream *substream, int cmd)
{
	struct tegra_runtime_data *prtd = substream->runtime->private_data;
	int i, ret = 0;

	switch (cmd) {
	case SNDRV_PCM_TRIGGER_START:
	case SNDRV_PCM_TRIGGER_RESUME:
	case SNDRV_PCM_TRIGGER_PAUSE_RELEASE:
		if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
			prtd->state = STATE_INIT;
			prtd->dma_state = STATE_INIT;
			tegra_pcm_play(prtd);	/* dma enqueue req1 */
			tegra_pcm_play(prtd);	/* dma enqueue req2 */
		} else {
			/* start recording */
			prtd->state = STATE_INIT;
			prtd->dma_state = STATE_INIT;
			tegra_pcm_capture(prtd);	/* dma enqueue req1 */
			tegra_pcm_capture(prtd);	/* dma enqueue req2 */
		}
		break;
	case SNDRV_PCM_TRIGGER_STOP:
	case SNDRV_PCM_TRIGGER_SUSPEND:
	case SNDRV_PCM_TRIGGER_PAUSE_PUSH:
		prtd->state = STATE_ABORT;
		prtd->dma_state = STATE_ABORT;
		tegra_dma_cancel(prtd->dma_chan);
		if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
			if (prtd->dma_chan) {
				for (i = 0; i < DMA_REQ_QCOUNT; i++)
					tegra_dma_dequeue_req(prtd->dma_chan,
							      &prtd->
							      dma_req[i]);
				prtd->dma_reqid_head = 0;
				prtd->dma_reqid_tail = DMA_REQ_QCOUNT - 1;

			}
		} else {
			if (prtd->dma_chan) {
				for (i = 0; i < DMA_REQ_QCOUNT; i++)
					tegra_dma_dequeue_req(prtd->dma_chan,
							      &prtd->
							      dma_req[i]);
				prtd->dma_reqid_head = 0;
				prtd->dma_reqid_tail = DMA_REQ_QCOUNT - 1;
			}
		}
		break;
	default:
		ret = -EINVAL;
		break;
	}
	return ret;
}

static snd_pcm_uframes_t tegra_pcm_pointer(struct snd_pcm_substream *substream)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct tegra_runtime_data *prtd = runtime->private_data;
	snd_pcm_uframes_t size;
	size = (prtd->period_index * runtime->period_size) +
	    bytes_to_frames(runtime,
			    tegra_dma_get_transfer_count(prtd->dma_chan,
							 &prtd->dma_req[prtd->
									dma_reqid_head],
							 false));
	return (size);
}

static int tegra_pcm_open(struct snd_pcm_substream *substream)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct tegra_runtime_data *prtd = runtime->private_data;
	struct tegra_i2s_info *info = prtd->dai->private_data;
	int i, ret = 0;

	if (!prtd) {
		pr_err("%s: Null prtd data \n", __func__);
		return -1;
	}

	prtd->audio_sync_clk = i2s_get_clock_by_name("audio_2x");
	if (!prtd->audio_sync_clk) {
		pr_err("%s: could not get audio_2x clock\n", __func__);
		return -EIO;
	}
	clk_enable(prtd->audio_sync_clk);
	prtd->state = STATE_INVALID;
	for (i = 0; i < DMA_REQ_QCOUNT; i++) {
		setup_dma_request(substream,
				  &prtd->dma_req[i],
				  dma_complete_callback, prtd);
	}

	prtd->dma_chan =
	    tegra_dma_allocate_channel(TEGRA_DMA_MODE_CONTINUOUS_DOUBLE);
	if (IS_ERR(prtd->dma_chan)) {
		pr_err("%s: could not allocate DMA channel for I2S: %ld\n",
		       __func__, PTR_ERR(prtd->dma_chan));
		ret = PTR_ERR(prtd->dma_chan);
		goto fail;
	}

	/* Set HW params now that initialization is complete */
	if (!info->pdata->tdm_enable) {
		snd_soc_set_runtime_hwparams(substream,
					     &tegra_pcm_hardware_i2s);
	} else {
		if (info->pdata->num_slots == 4)
			snd_soc_set_runtime_hwparams(substream,
						     &tegra_pcm_hardware_tdm1);
		else
			snd_soc_set_runtime_hwparams(substream,
						     &tegra_pcm_hardware_tdm2);
	}

	goto end;

fail:
	prtd->state = STATE_EXIT;

	if (prtd->dma_chan) {
		tegra_dma_flush(prtd->dma_chan);
		tegra_dma_free_channel(prtd->dma_chan);
	}

	/* set pins state to tristate */

	kfree(prtd);

end:
	return ret;
}

static int tegra_pcm_close(struct snd_pcm_substream *substream)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct tegra_runtime_data *prtd = runtime->private_data;
	int i;

	if (!prtd) {
		printk(KERN_ERR "tegra_pcm_close called with prtd == NULL\n");
		return 0;
	}

	prtd->state = STATE_EXIT;
	if (prtd->dma_chan) {
		prtd->dma_state = STATE_EXIT;
		for (i = 0; i < DMA_REQ_QCOUNT; i++)
			tegra_dma_dequeue_req(prtd->dma_chan,
					      &prtd->dma_req[i]);
		tegra_dma_flush(prtd->dma_chan);
		tegra_dma_free_channel(prtd->dma_chan);
		prtd->dma_chan = NULL;
		prtd->dma_reqid_head = 0;
		prtd->dma_reqid_tail = DMA_REQ_QCOUNT - 1;
	}
	kfree(prtd);

	return 0;
}

static int tegra_pcm_mmap(struct snd_pcm_substream *substream,
			  struct vm_area_struct *vma)
{
	struct snd_pcm_runtime *runtime = substream->runtime;

	return dma_mmap_writecombine(substream->pcm->card->dev, vma,
				     runtime->dma_area,
				     runtime->dma_addr, runtime->dma_bytes);
}

static struct snd_pcm_ops tegra_pcm_ops = {
	.open = tegra_pcm_open,
	.close = tegra_pcm_close,
	.ioctl = snd_pcm_lib_ioctl,
	.hw_params = tegra_pcm_hw_params,
	.hw_free = tegra_pcm_hw_free,
	.prepare = tegra_pcm_prepare,
	.trigger = tegra_pcm_trigger,
	.pointer = tegra_pcm_pointer,
	.mmap = tegra_pcm_mmap,
};

static int tegra_pcm_preallocate_dma_buffer(struct snd_pcm *pcm, int stream)
{
	struct snd_pcm_substream *substream = pcm->streams[stream].substream;
	struct snd_dma_buffer *buf = &substream->dma_buffer;
	size_t size;

	size = DMA_BUFFER_SIZE;
	buf->dev.type = SNDRV_DMA_TYPE_DEV;
	buf->dev.dev = pcm->card->dev;
	buf->private_data = NULL;
	buf->area = dma_alloc_writecombine(pcm->card->dev, size,
					   &buf->addr, GFP_KERNEL);
	buf->bytes = size;
	return 0;
}

static void tegra_pcm_deallocate_dma_buffer(struct snd_pcm *pcm, int stream)
{
	struct snd_pcm_substream *substream = pcm->streams[stream].substream;
	struct snd_dma_buffer *buf = &substream->dma_buffer;

	for (stream = 0; stream < 2; stream++) {
		substream = pcm->streams[stream].substream;
		if (!substream)
			continue;

		buf = &substream->dma_buffer;
		if (!buf->area)
			continue;

		dma_free_writecombine(pcm->card->dev, buf->bytes,
				      buf->area, buf->addr);
		buf->area = NULL;
	}
}

static void tegra_pcm_free_dma_buffers(struct snd_pcm *pcm)
{
	struct snd_pcm_substream *substream;
	struct snd_dma_buffer *buf;
	int stream;

	for (stream = 0; stream < 2; stream++) {
		substream = pcm->streams[stream].substream;
		buf = &substream->dma_buffer;
		if (!buf) {
			printk(KERN_ERR "no buffer %d \n", stream);
			continue;
		}
		tegra_pcm_deallocate_dma_buffer(pcm, stream);
	}

}

static u64 tegra_dma_mask = DMA_BIT_MASK(32);

static int tegra_pcm_new(struct snd_card *card,
			 struct snd_soc_dai *dai, struct snd_pcm *pcm)
{
	int ret = 0;

	if (!card->dev->dma_mask)
		card->dev->dma_mask = &tegra_dma_mask;
	if (!card->dev->coherent_dma_mask)
		card->dev->coherent_dma_mask = 0xffffffff;

	if (dai->playback.channels_min) {
		ret = tegra_pcm_preallocate_dma_buffer(pcm,
						       SNDRV_PCM_STREAM_PLAYBACK);
		if (ret)
			goto out;
	}

	if (dai->capture.channels_min) {
		ret = tegra_pcm_preallocate_dma_buffer(pcm,
						       SNDRV_PCM_STREAM_CAPTURE);
		if (ret)
			goto out;
	}
out:
	return ret;
}

struct snd_soc_platform tegra_soc_platform = {
	.name = "tegra-audio",
	.pcm_ops = &tegra_pcm_ops,
	.pcm_new = tegra_pcm_new,
	.pcm_free = tegra_pcm_free_dma_buffers,
};

#ifdef CONFIG_EMBEDDED_TEGRA_ALSA_SPDIF

static inline phys_addr_t spdif_get_fifo_phy_base(void)
{
	return TEGRA_SPDIF_BASE + SPDIF_DATA_IN_0;
}

static inline phys_addr_t spdif_out_get_fifo_phy_base(void)
{
	return TEGRA_SPDIF_BASE + SPDIF_DATA_OUT_0;
}

int setup_spdif_dma_request(struct snd_pcm_substream *substream,
			    struct tegra_dma_req *req,
			    void (*dma_callback) (struct tegra_dma_req * req),
			    void *dma_data)
{

	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_dai *cpu_dai = rtd->dai->cpu_dai;
	struct tegra_spdif_info *info = cpu_dai->private_data;

	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
		req->to_memory = false;
		req->dest_addr = spdif_out_get_fifo_phy_base();
		req->dest_wrap = 4;
		req->source_wrap = 0;
	} else {
		req->to_memory = true;
		req->source_addr = spdif_get_fifo_phy_base();
		req->dest_wrap = 0;
		req->source_wrap = 4;
	}

	req->complete = dma_callback;
	req->dev = dma_data;
	req->source_bus_width = 32;
	req->req_sel = info->dma_req_sel;
	req->dest_bus_width = 32;

	return 0;
}

static const struct snd_pcm_hardware tegra_pcm_hardware_spdif = {
	.info = SNDRV_PCM_INFO_INTERLEAVED | SNDRV_PCM_INFO_PAUSE |
	    SNDRV_PCM_INFO_RESUME | SNDRV_PCM_INFO_MMAP |
	    SNDRV_PCM_INFO_MMAP_VALID,
	.formats = SNDRV_PCM_FMTBIT_S16_LE |
	    SNDRV_PCM_FMTBIT_S24_LE | SNDRV_PCM_FMTBIT_S32_LE,
	.channels_min = 2,
	.channels_max = 2,
	.buffer_bytes_max = 32 * 1024,
	.period_bytes_min = 8192,
	.period_bytes_max = 8192,
	.periods_min = 4,
	.periods_max = 8,
	.fifo_size = 4,
};

static int tegra_spdif_pcm_open(struct snd_pcm_substream *substream)
{

	struct snd_pcm_runtime *runtime = substream->runtime;
	struct tegra_runtime_data *prtd = runtime->private_data;
	int i, ret = 0;

	if (!prtd) {
		pr_err("%s: Null prtd data \n", __func__);
		return -1;
	}

	for (i = 0; i < DMA_REQ_QCOUNT; i++)
		setup_spdif_dma_request(substream, &prtd->dma_req[i],
					dma_complete_callback, prtd);

	prtd->dma_chan = tegra_dma_allocate_channel(TEGRA_DMA_MODE_CONTINUOUS);
	if (IS_ERR(prtd->dma_chan)) {
		pr_err("%s: could not allocate DMA channel for SPDIF: %ld\n",
		       __func__, PTR_ERR(prtd->dma_chan));
		ret = PTR_ERR(prtd->dma_chan);
		goto fail;
	}
	/* Set HW params now that initialization is complete */

	snd_soc_set_runtime_hwparams(substream, &tegra_pcm_hardware_spdif);

	goto end;

fail:	prtd->state = STATE_EXIT;

	if (prtd->dma_chan) {
		tegra_dma_flush(prtd->dma_chan);
		tegra_dma_free_channel(prtd->dma_chan);
	}

	/* set pins state to tristate */

	kfree(prtd);

end:
	return ret;
}

static struct snd_pcm_ops tegra_spdif_pcm_ops = {
	.open = tegra_spdif_pcm_open,
	.close = tegra_pcm_close,
	.ioctl = snd_pcm_lib_ioctl,
	.hw_params = tegra_pcm_hw_params,
	.hw_free = tegra_pcm_hw_free,
	.prepare = tegra_pcm_prepare,
	.trigger = tegra_pcm_trigger,
	.pointer = tegra_pcm_pointer,
	.mmap = tegra_pcm_mmap,
};

struct snd_soc_platform tegra_spdif_soc_platform = {.name = "tegra-audio",
	.pcm_ops = &tegra_spdif_pcm_ops,.pcm_new = tegra_pcm_new,
	.pcm_free = tegra_pcm_free_dma_buffers,
};

#endif // CONFIG_EMBEDDED_TEGRA_ALSA_SPDIF

EXPORT_SYMBOL_GPL(tegra_soc_platform);

static int __init tegra_soc_platform_init(void)
{
	int ret = 0;

	ret = snd_soc_register_platform(&tegra_soc_platform);
	if (ret)
		return ret;

#ifdef CONFIG_EMBEDDED_TEGRA_ALSA_SPDIF
	ret = snd_soc_register_platform(&tegra_spdif_soc_platform);
	if (ret)
		return ret;
#endif

	return (ret);

}

module_init(tegra_soc_platform_init);

static void __exit tegra_soc_platform_exit(void)
{
	snd_soc_unregister_platform(&tegra_soc_platform);

#ifdef CONFIG_EMBEDDED_TEGRA_ALSA_SPDIF
	snd_soc_unregister_platform(&tegra_spdif_soc_platform);
#endif
}

module_exit(tegra_soc_platform_exit);

MODULE_DESCRIPTION("Tegra PCM DMA module");
MODULE_LICENSE("GPL");
