#include "OBSMainWindow.h"

OBSMainWindow::OBSMainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
}

void OBSMainWindow::OBSInit()
{
	if (!obs_startup("zh-CN",nullptr, nullptr)) {
		QMessageBox::warning(nullptr, String("错误"), String("启动OBS失败"));
		return;
	}
	//------------视频预览---------------//
	//初始化音频
	if (!ResetAudio()) {
		QMessageBox::warning(nullptr,String("错误"),String("初始化音频失败"));
		return;
	}
	//初始化视频
	int ret = ResetVidio();
	if (ret != OBS_VIDEO_SUCCESS) {
		QMessageBox::warning(nullptr, String("错误"), String("初始化视频失败"));
		return;
	}
	obs_load_all_modules();		// @xp : 初始化所有的插件动态库

	//添加一个场景
	obs_scene* scence = obs_scene_create("default_sence");
	obs_source *scenceSource = obs_scene_get_source(scence);
	obs_set_output_source(0, scenceSource);
}

bool OBSMainWindow::ResetAudio()
{
	//设置音频参数
	struct obs_audio_info ai;

	ai.samples_per_sec = 44100;	//设置音频采样率
	ai.speakers = SPEAKERS_STEREO;	//设置声道
	//end
	return obs_reset_audio(&ai);
}

int OBSMainWindow::ResetVidio()
{
	//设置视频参数
	struct obs_video_info ovi;
	int ret;

	ovi.graphics_module = "libobs-d3d11.dll";	//设置渲染模式
	ovi.fps_den = 1;		//设置分母
	ovi.fps_num = viedo_quality_levels[2].fps_num;//设置分子
	ovi.base_width = viedo_quality_levels[2].output_width;
	ovi.base_height = viedo_quality_levels[2].output_height;
	ovi.output_width = viedo_quality_levels[2].output_width;
	ovi.output_height = viedo_quality_levels[2].output_height;
	ovi.colorspace = VIDEO_CS_601;		
	ovi.range = VIDEO_RANGE_PARTIAL;		
	ovi.adapter = 0;		
	ovi.gpu_conversion = true;		
	ovi.scale_type = OBS_SCALE_BICUBIC;	//obs视频缩放算法
	//end

	ret = obs_reset_video(&ovi);
	if (ret != OBS_VIDEO_SUCCESS) {
		/* Try OpenGL if DirectX fails on windows */
			ovi.graphics_module = "libobs-opengl.dll";
			ret = obs_reset_video(&ovi);
	}

	return ret;
}
