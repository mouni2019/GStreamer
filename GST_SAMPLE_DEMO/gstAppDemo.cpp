

/*!
 * \file gstAppDemo.cpp
 * \brief Demonstrates Buffer handling in Gst application
// @cond AUTHOR_DETAILS
 * \author Mounika Botcha <mounika.botcha20@gmail.com>
 * \date Sep 30th, 2022
// @endcond
*/



#include <gst/gst.h>
#include <gst/app/app.h>
#include <cstdlib>
#include <unistd.h>
#include <opencv2/opencv.hpp>


#include <iostream>
#include <gst/video/video.h>

using namespace std;
using namespace cv;

int width = 640;
int height = 480;

#define QLEN 600
#define BOX_WIDTH 400

#define BOX_HEIGHT 100

int main()
{
 GstElement *sink_pipeline, *src_pipeline, *convert;
 GstVideoInfo vinfo;
 GstBus *bus;
 GstMessage *msg;
 GstAppSrc *app_src;
 GstAppSink *app_sink;

 gst_init(NULL,NULL);

 //gchar *description = g_strdup_printf("appsrc is-live=true name=display size=%d ! videoconvert name=convert ! video/x-raw, width=%d, height=%d,framerate=30/1,format=RGBA ! xvimagesink display=output", QLEN*width*height*4, width, height);
 //gchar *description = g_strdup_printf("appsrc is-live=true name=display size=%d ! videoconvert name=convert ! video/x-raw, width=%d, height=%d,framerate=30/1,format=RGBA ! x264enc ! avimux ! filesink location=out.avi", QLEN*width*height*4, width, height);
 gchar *description = g_strdup_printf("appsrc name=app_source ! videoconvert! video/x-raw, framerate=30000/1001 ! videoscale ! video/x-raw,width=640,height=480 ! appsink name=app_sink");
 src_pipeline = gst_parse_launch(description, NULL);
 app_src =  GST_APP_SRC(gst_bin_get_by_name(GST_BIN(src_pipeline), "app_source"));
 g_object_set(app_src, "is-live", true, NULL);
 gchar* caps =  "video/x-raw, format=BGR, framerate=30000/1001, width=1280, height=720";
 GstCaps *new_caps = gst_caps_from_string (caps);
 gst_app_src_set_caps(app_src, new_caps);
 app_sink =  GST_APP_SINK(gst_bin_get_by_name(GST_BIN(src_pipeline), "app_sink"));

 bus = gst_element_get_bus (src_pipeline);


 gst_element_set_state(src_pipeline, GST_STATE_PLAYING);
 /* Outer loop */

 cv::VideoCapture cap("walking.mp4");
 if(!cap.isOpened())
     printf("Could not open file \n");
 cv::Mat frame;

 bool loop_exit = false;
 while(!loop_exit) {

    cap >> frame;
    if (frame.empty()) {
      break;
    }

    /*inner loop */

  GstMessage *message;
  GError *err;
  while (!loop_exit && (message = gst_bus_pop_filtered (bus, GST_MESSAGE_ANY))) {
  if (GST_MESSAGE_TYPE (message) == GST_MESSAGE_ERROR) {
      g_autofree gchar *debug = NULL;
      gst_message_parse_error (message, &err, &debug);
      fprintf (stderr, "GStreamer Error: %s\n%s\n", err->message, debug);
      loop_exit = true;
        }
  else if (GST_MESSAGE_TYPE (message) == GST_MESSAGE_EOS) {
          fprintf (stderr, "End of stream\n");
          loop_exit = true;
        }
  
  gst_message_unref (message);
     }
  GstBuffer *buff = gst_buffer_new_allocate(NULL, frame.cols*frame.rows*3, NULL);
  GstMapInfo info;
  gst_buffer_map(buff, &info, GST_MAP_READ);
  unsigned char* cbuf = info.data;
  memmove(cbuf, frame.data, frame.cols*frame.rows*3);
  gst_buffer_unmap(buff, &info);
  if(GST_FLOW_OK != gst_app_src_push_buffer(GST_APP_SRC(app_src), buff))
     fprintf(stderr, "****************************** ERROR *************\n\n\n");

  GstBuffer *outbuf;
  GstVideoInfo vinfo;
  GstSample *sample = gst_app_sink_pull_sample(app_sink);
  if(sample) {
      outbuf = gst_sample_get_buffer(sample);
      if(!outbuf)
	fprintf(stderr, "unable to get buffer\n");
      GstCaps * caps;
      caps = gst_sample_get_caps (sample);

      gst_video_info_from_caps (&vinfo, caps);
      }
	 //get the sample from appsink
  GstMapInfo outinfo;
  gst_buffer_map(outbuf, &outinfo, GST_MAP_READ);
  unsigned char* outcbuf = outinfo.data;
  Mat outframe(height,width,CV_8UC3);
  memmove( outframe.data,outcbuf, outframe.cols*outframe.rows*3);
  gst_buffer_unmap(outbuf, &outinfo);
  //Adding draw rrectangular box on frame 
  int center_row=height/2;
  int center_col=width/2;
  point p1(center_row-(BOX_WIDTH/2),center_col-(BOX_HEIGHT/2));
  Point p2(center_row+(BOX_WIDTH/2),center_col+(BOX_HEIGHT/2));
  rectangle(outframe,p1,p2,Scalar(0,255,0),3);
  const char *text = " Blaize - Gstreamer Sample App";
  cv::putText (outframe, text, cv::Point(center_row-(BOX_WIDTH/2), center_col), cv::FONT_HERSHEY_COMPLEX_SMALL, 0.8, cv::Scalar(255, 0, 0), 1, LINE_AA);
  imshow("output",outframe);
  waitKey(10);
  }

  gst_element_set_state(src_pipeline, GST_STATE_NULL);
  gst_object_unref (src_pipeline);
  }
