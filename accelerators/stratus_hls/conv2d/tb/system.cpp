// Copyright (c) 2011-2019 Columbia University, System Level Design Group
// SPDX-License-Identifier: Apache-2.0

#include <sstream>
#include "system.hpp"

// Process
void system_t::config_proc()
{

    // Reset
    {
        conf_done.write(false);
        conf_info.write(conf_info_t());
        wait();
    }

    ESP_REPORT_INFO("reset done");

    // Config
    load_memory();
    {
        conf_info_t config;
        // Custom configuration
        /* <<--params-->> */
        config.n_channels = channels;
        config.feature_map_height = height;
        config.feature_map_width = width;
        config.n_filters = num_filters;
        config.filter_height = kernel_h;
        config.filter_width = kernel_w;
        config.pad_h = pad_h;
        config.pad_w = pad_w;
        config.stride_h = stride_h;
        config.stride_w = stride_w;
        config.dilation_h = dilation_h;
        config.dilation_w = dilation_w;

        wait(); conf_info.write(config);
        conf_done.write(true);
    }

    ESP_REPORT_INFO("config done");

    // Compute
    // {
    //     // Print information about begin time
    //     sc_time begin_time = sc_time_stamp();
    //     ESP_REPORT_TIME(begin_time, "BEGIN - conv2d");

    //     // Wait the termination of the accelerator
    //     do { wait(); } while (!acc_done.read());
    //     debug_info_t debug_code = debug.read();

    //     // Print information about end time
    //     sc_time end_time = sc_time_stamp();
    //     ESP_REPORT_TIME(end_time, "END - conv2d");

    //     esc_log_latency(sc_object::basename(), clock_cycle(end_time - begin_time));
    //     wait(); conf_done.write(false);
    // }
    sw_conv_layer_fpdata(hw_input, channels, height, width, kernel_h, kernel_w, pad_h, pad_w,
		  stride_h, stride_w, dilation_h, dilation_w, num_filters, hw_weights, hw_output);
    ESP_REPORT_INFO("HW Accelerator done");

    // Compute in SW
    sw_conv_layer(sw_input, channels, height, width, kernel_h, kernel_w, pad_h, pad_w,
		  stride_h, stride_w, dilation_h, dilation_w, num_filters, sw_weights, sw_output);
    ESP_REPORT_INFO("SW done");

#ifdef TINY
    print_hw_image("output-hw", hw_output, num_filters, height, width);
    print_sw_image("output-sw", sw_output, num_filters, height, width);
#endif

    // printf("Performance: data-in : moved %u (%u bytes) / memory footprint %u (%u bytes) / PLM locality %.2f%%",
    // 	   data_in_movement, data_in_movement * sizeof(FPDATA), channels * height * width, channels * height *
    // 	   width * sizeof(FPDATA),100*(channels * height * width)/((float)data_in_movement));
    // printf("Performance: weights : moved %u (%u bytes) / memory footprint %u (%u bytes) / PLM locality %.2f%%",
    // 	   weights_movement, weights_movement * sizeof(FPDATA), num_filters * channels * kernel_h * kernel_w,
    // 	   num_filters * channels * kernel_h * kernel_w * sizeof(FPDATA),
    // 	   100*(num_filters * channels * kernel_h * kernel_w)/((float)weights_movement));
    // printf("Performance: data-out: moved %u (%u bytes) / memory footprint %u (%u bytes) / PLM locality %.2f%%",
    // 	   data_out_movement, data_out_movement * sizeof(FPDATA), num_filters * height * width,  num_filters *
    // 	   height * width * sizeof(FPDATA), 100*(num_filters*height*width)/((float)data_out_movement));


    // Validate
    {
        // dump_memory(); // store the output in more suitable data structure if needed
        // check the results with the golden model
        if (validate())
        {
            ESP_REPORT_ERROR("validation failed!");
        } else
        {
            ESP_REPORT_INFO("validation passed!");
        }
    }

    // Conclude
    {
        sc_stop();
    }
}

// Functions
void system_t::load_memory()
{
    // Optional usage check
#ifdef CADENCE
    if (esc_argc() != 1)
    {
        ESP_REPORT_INFO("usage: %s\n", esc_argv()[0]);
        sc_stop();
    }
#endif

    hw_input = (FPDATA*)malloc(input_max_size * sizeof(FPDATA));
    hw_weights = (FPDATA*)malloc(weights_max_size * sizeof(FPDATA));
    hw_output = (FPDATA*)malloc(output_max_size * sizeof(FPDATA));

    sw_input = (float*) malloc(input_max_size * sizeof(float));
    sw_weights = (float*) malloc(weights_max_size * sizeof(float));
    sw_output = (float*) malloc(output_max_size * sizeof(float));

    init_image(hw_input, sw_input, channels, height, width, true);
    init_weights(hw_weights, sw_weights, num_filters, channels, kernel_h, kernel_w, true);

    print_hw_image("input-hw", hw_input, channels, height, width);
    print_sw_image("input-sw", sw_input, channels, height, width);

    print_hw_weights("weights-hw", hw_weights, num_filters, channels, kernel_h, kernel_w);
    print_sw_weights("weights-sw", sw_weights, num_filters, channels, kernel_h, kernel_w);

//     // Input data and golden output (aligned to DMA_WIDTH makes your life easier)
// #if (DMA_WORD_PER_BEAT == 0)
//     in_words_adj = channels * height * width;
//     out_words_adj = channels * height * width;
// #else
//     in_words_adj = round_up(channels * height * width, DMA_WORD_PER_BEAT);
//     out_words_adj = round_up(channels * height * width, DMA_WORD_PER_BEAT);
// #endif

//     in_size = in_words_adj * (1);
//     out_size = out_words_adj * (1);

//     in = new int32_t[in_size];
//     for (int i = 0; i < 1; i++)
//         for (int j = 0; j < channels * height * width; j++)
//             in[i * in_words_adj + j] = (int32_t) j;

//     // Compute golden output
//     gold = new int32_t[out_size];
//     for (int i = 0; i < 1; i++)
//         for (int j = 0; j < channels * height * width; j++)
//             gold[i * out_words_adj + j] = (int32_t) j;

//     // Memory initialization:
// #if (DMA_WORD_PER_BEAT == 0)
//     for (int i = 0; i < in_size; i++)  {
//         sc_dt::sc_bv<DATA_WIDTH> data_bv(in[i]);
//         for (int j = 0; j < DMA_BEAT_PER_WORD; j++)
//             mem[DMA_BEAT_PER_WORD * i + j] = data_bv.range((j + 1) * DMA_WIDTH - 1, j * DMA_WIDTH);
//     }
// #else
//     for (int i = 0; i < in_size / DMA_WORD_PER_BEAT; i++)  {
//         sc_dt::sc_bv<DMA_WIDTH> data_bv(in[i]);
//         for (int j = 0; j < DMA_WORD_PER_BEAT; j++)
//             data_bv.range((j+1) * DATA_WIDTH - 1, j * DATA_WIDTH) = in[i * DMA_WORD_PER_BEAT + j];
//         mem[i] = data_bv;
//     }
// #endif

    ESP_REPORT_INFO("load memory completed");
}

void system_t::dump_memory()
{
    // Get results from memory
    out = new int32_t[out_size];
    uint32_t offset = in_size;

#if (DMA_WORD_PER_BEAT == 0)
    offset = offset * DMA_BEAT_PER_WORD;
    for (int i = 0; i < out_size; i++)  {
        sc_dt::sc_bv<DATA_WIDTH> data_bv;

        for (int j = 0; j < DMA_BEAT_PER_WORD; j++)
            data_bv.range((j + 1) * DMA_WIDTH - 1, j * DMA_WIDTH) = mem[offset + DMA_BEAT_PER_WORD * i + j];

        out[i] = data_bv.to_int64();
    }
#else
    offset = offset / DMA_WORD_PER_BEAT;
    for (int i = 0; i < out_size / DMA_WORD_PER_BEAT; i++)
        for (int j = 0; j < DMA_WORD_PER_BEAT; j++)
            out[i * DMA_WORD_PER_BEAT + j] = mem[offset + i].range((j + 1) * DATA_WIDTH - 1, j * DATA_WIDTH).to_int64();
#endif

    ESP_REPORT_INFO("dump memory completed");
}

int system_t::validate()
{
    // Check for mismatches
    uint32_t errors = 0;

    errors = _validate(hw_output, sw_output, num_filters * height * width);

    // for (int i = 0; i < 1; i++)
    //     for (int j = 0; j < channels * height * width; j++)
    //         if (gold[i * out_words_adj + j] != out[i * out_words_adj + j])
    //             errors++;

    // delete [] in;
    // delete [] out;
    // delete [] gold;

    return errors;
}