#include "OBSMainWindow.h"

OBSMainWindow::OBSMainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
}

void OBSMainWindow::OBSInit()
{
	if (!obs_startup("zh-CN",nullptr, nullptr)) {
		QMessageBox::warning(nullptr, String("����"), String("����OBSʧ��"));
		return;
	}
	//------------��ƵԤ��---------------//
	//��ʼ����Ƶ
	if (!ResetAudio()) {
		QMessageBox::warning(nullptr,String("����"),String("��ʼ����Ƶʧ��"));
		return;
	}
	//��ʼ����Ƶ
	int ret = ResetVidio();
	if (ret != OBS_VIDEO_SUCCESS) {
		QMessageBox::warning(nullptr, String("����"), String("��ʼ����Ƶʧ��"));
		return;
	}
	obs_load_all_modules();		// @xp : ��ʼ�����еĲ����̬��

	//���һ������
	obs_scene* scence = obs_scene_create("default_sence");
	obs_source *scenceSource = obs_scene_get_source(scence);
	obs_set_output_source(0, scenceSource);
}

bool OBSMainWindow::ResetAudio()
{
	//������Ƶ����
	struct obs_audio_info ai;

	ai.samples_per_sec = 44100;	//������Ƶ������
	ai.speakers = SPEAKERS_STEREO;	//��������
	//end
	return obs_reset_audio(&ai);
}

int OBSMainWindow::ResetVidio()
{
	//������Ƶ����
	struct obs_video_info ovi;
	int ret;

	ovi.graphics_module = "libobs-d3d11.dll";	//������Ⱦģʽ
	ovi.fps_den = 1;		//���÷�ĸ
	ovi.fps_num = viedo_quality_levels[2].fps_num;//���÷���
	ovi.base_width = viedo_quality_levels[2].output_width;
	ovi.base_height = viedo_quality_levels[2].output_height;
	ovi.output_width = viedo_quality_levels[2].output_width;
	ovi.output_height = viedo_quality_levels[2].output_height;
	ovi.colorspace = VIDEO_CS_601;		
	ovi.range = VIDEO_RANGE_PARTIAL;		
	ovi.adapter = 0;		
	ovi.gpu_conversion = true;		
	ovi.scale_type = OBS_SCALE_BICUBIC;	//obs��Ƶ�����㷨
	//end

	ret = obs_reset_video(&ovi);
	if (ret != OBS_VIDEO_SUCCESS) {
		/* Try OpenGL if DirectX fails on windows */
			ovi.graphics_module = "libobs-opengl.dll";
			ret = obs_reset_video(&ovi);
	}

	return ret;
}
