#pragma once
#include "precompile.h"

#include <QtWidgets/QMainWindow>
#include <QMessageBox>
#include "ui_OBSMainWindow.h"


#include "obs.h"
#include "util/util.hpp"

#ifdef __APPLE__
#define INPUT_AUDIO_SOURCE  "coreaudio_input_capture"
#define OUTPUT_AUDIO_SOURCE "coreaudio_output_capture"
#define FFMPEG_SOURCE "display_capture"
#elif _WIN32
constexpr auto INPUT_AUDIO_SOURCE = "wasapi_input_capture";
constexpr auto OUTPUT_AUDIO_SOURCE = "wasapi_output_capture";
constexpr auto FFMPEG_SOURCE = "ffmpeg_source";
constexpr auto WINDOW_CAPTURE = "window_capture";
constexpr auto MONITOR_CAPTURE = "monitor_capture";
#endif
struct video_quality
{
	uint32_t fps_num;       /**< Output FPS numerator */
	uint32_t output_width;  /**< Output width */
	uint32_t output_height; /**< Output height */
	int video_bitrate;
	QString name;
	int level;
	QString bitrateName;

};

const video_quality viedo_quality_levels[4] = {
	{ 15, 480, 270, 172, QString::fromLocal8Bit("流畅"),0, "250" },
	{ 15, 800, 450, 250, QString::fromLocal8Bit("标清"),0, "500" },
	{ 25, 1280, 720, 1000, QString::fromLocal8Bit("高清"),0, "800" },
	{ 25, 1920, 1080, 2000, QString::fromLocal8Bit("超清"),0, "1200" }
};

class OBSMainWindow : public QMainWindow
{
	Q_OBJECT

public:
	OBSMainWindow(QWidget *parent = Q_NULLPTR);

	void OBSInit();

	bool ResetAudio();

	int ResetVidio();

	void PlayVideo(QString filePath);

	void CaptureWindow();

	void DrawBackdrop(float cx, float cy);

	static void DrawMainPreview(void *data, uint32_t cx, uint32_t cy);	//绘制预览窗口的回调函数

private:
	Ui::OBSMainWindowClass ui;
	
	ConfigFile basicConfig;
};
