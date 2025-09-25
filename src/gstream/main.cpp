// main.cpp
#include <glib.h>
#include <gst/gst.h>
#include <iostream>

static gboolean stop_pipeline(gpointer user_data) {
    GstPipeline* pipeline = static_cast<GstPipeline*>(user_data);

    std::cout << "Dump pipeline" << std::endl;
    GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(pipeline), GST_DEBUG_GRAPH_SHOW_ALL, "my-pipeline");

    std::cout << "Stopping pipeline after 10 seconds..." << std::endl;
    gst_element_send_event(GST_ELEMENT(pipeline), gst_event_new_eos());

    return G_SOURCE_REMOVE;
}

void bus_callback(GstBus*, GstMessage* msg, gpointer data) {
    GMainLoop* loop = static_cast<GMainLoop*>(data);
    switch (GST_MESSAGE_TYPE(msg)) {
    case GST_MESSAGE_ERROR: {
        GError* err;
        gchar* debug_info;
        gst_message_parse_error(msg, &err, &debug_info);
        std::cerr << "Error: " << err->message << std::endl;
        std::cerr << "Debug info: " << (debug_info ? debug_info : "none") << std::endl;
        g_clear_error(&err);
        g_free(debug_info);
        g_main_loop_quit(loop);
        break;
    }
    case GST_MESSAGE_EOS:
        std::cout << "End-Of-Stream reached." << std::endl;
        g_main_loop_quit(loop);
        break;
    default:
        break;
    }
}

int main(int argc, char* argv[]) {
    gst_init(&argc, &argv);

    const gchar* pipeline_desc =
        "filesrc location=/home/dev/Documents/sources/test-hailort/input.mp4 ! "
        "decodebin ! "
        "videoscale ! "
        "videoconvert ! "
        "videorate ! "
        "video/x-raw,format=RGB,width=640,height=640,framerate=30/1 ! "
        "hailonet hef-path=/home/dev/Documents/sources/test-hailort/model/yolov8m.hef ! "
        "queue ! "
        "hailofilter so-path=/home/dev/Documents/sources/test-hailort/build/libhailo-gst-postproc.so "
        "name=detection_hailofilter qos=false function_name=detection_postprocess ! "
        "queue ! "
        "videoconvert ! "
        "videoscale ! "
        "video/x-raw,format=RGB,pixel-aspect-ratio=1/1 ! "
        "hailooverlay ! "
        "videoconvert ! "
        "x264enc tune=zerolatency bitrate=6000 speed-preset=ultrafast key-int-max=60 ! "
        "mp4mux ! "
        "filesink location=./output_with_hailo.mp4 ";
    GError* error = nullptr;
    GstElement* pipeline = gst_parse_launch(pipeline_desc, &error);
    if (!pipeline) {
        std::cerr << "Failed to create pipeline: " << error->message << std::endl;
        g_clear_error(&error);
        return -1;
    }

    GMainLoop* loop = g_main_loop_new(nullptr, FALSE);
    GstBus* bus = gst_element_get_bus(pipeline);

    gst_bus_add_signal_watch(bus);
    g_signal_connect(bus, "message", G_CALLBACK(bus_callback), loop);

    // Used only for inference only first 10 seconds
    // If you need to full stream inference, comment this line
    g_timeout_add(10000, stop_pipeline, pipeline);

    GstStateChangeReturn ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        std::cerr << "Failed to start pipeline." << std::endl;
        gst_object_unref(pipeline);
        return -1;
    }

    g_main_loop_run(loop);

    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(bus);
    gst_object_unref(pipeline);
    g_main_loop_unref(loop);

    return 0;
}
