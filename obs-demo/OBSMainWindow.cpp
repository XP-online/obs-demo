#include "OBSMainWindow.h"
#include <QDebug>

//输出日志的函数
void obs_log(int log_level, const char *msg, va_list args, void *param)
{
	char str[4096];
	vsnprintf(str, 4095, msg, args);

	qDebug() << str << "\n";

}
//obs-studio/UI/window-basic-main 中的函数
static inline bool HasAudioDevices(const char *source_id)
{
	const char *output_id = source_id;
	obs_properties_t *props = obs_get_source_properties(output_id);
	size_t count = 0;

	if (!props)
		return false;

	obs_property_t *devices = obs_properties_get(props, "device_id");
	if (devices)
		count = obs_property_list_item_count(devices);

	obs_properties_destroy(props);

	return count != 0;
}
//obs-studio/UI/window-basic-main 中的函数 
//添加音频源
void ResetAudioDevice(const char *sourceId, const char *deviceId,
	const char *deviceDesc, int channel)
{
	bool disable = deviceId && strcmp(deviceId, "disabled") == 0;
	obs_source_t *source;
	obs_data_t *settings;

	source = obs_get_output_source(channel);
	if (source) {
		if (disable) {
			obs_set_output_source(channel, nullptr);
		}
		else {
			settings = obs_source_get_settings(source);
			const char *oldId = obs_data_get_string(settings,
				"device_id");
			if (strcmp(oldId, deviceId) != 0) {
				obs_data_set_string(settings, "device_id",
					deviceId);
				obs_source_update(source, settings);
			}
			obs_data_release(settings);
		}

		obs_source_release(source);

	}
	else if (!disable) {
		settings = obs_data_create();
		obs_data_set_string(settings, "device_id", deviceId);
		source = obs_source_create(sourceId, deviceDesc, settings,
			nullptr);
		obs_data_release(settings);

		obs_set_output_source(channel, source);
		obs_source_release(source);
	}
}
void set_sceneitem_size(obs_sceneitem_t* sceneitem) 
{
	obs_video_info ovi;
	obs_get_video_info(&ovi);

	obs_transform_info itemInfo;
	vec2_set(&itemInfo.pos, 0.0f, 0.0f);
	vec2_set(&itemInfo.scale, 1.0f, 1.0f);
	itemInfo.rot = 0.0f;

	vec2_set(&itemInfo.bounds, float(ovi.base_width), float(ovi.base_height));
	itemInfo.bounds_type = OBS_BOUNDS_SCALE_INNER;
	itemInfo.alignment = OBS_ALIGN_LEFT | OBS_ALIGN_TOP;
	//itemInfo.bounds_alignment = OBS_ALIGN_CENTER;
	obs_sceneitem_set_info(sceneitem, &itemInfo);
}
void add_source_callback(void* data, obs_scene_t* scene) 
{
	obs_source* source = (obs_source*)data;
	obs_sceneitem_t* sceneitem = obs_scene_add(scene, source);
	obs_sceneitem_set_visible(sceneitem, true);
	set_sceneitem_size(sceneitem);
}
//添加视频源
void AddVideoSource(const char *id) {
	obs_source* source = obs_get_source_by_name(id);
	if (!source) {
		OBSData settings = obs_get_source_defaults("ffmpeg_source");
		obs_data_set_bool(settings, "is_local_file", true);
		obs_data_set_string(settings, "local_file", "D:\\boy.mp4");
		OBSSource source = obs_source_create("ffmpeg_source", String("视频文件").toUtf8(), settings, nullptr);
		obs_source* scene_source = obs_get_source_by_name("default_sence");
		obs_scene* scene = obs_scene_from_source(scene_source);
		obs_scene_atomic_update(scene, add_source_callback, source);
	} else {
		obs_source_release(source);
	}
}
OBSMainWindow::OBSMainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
}

void OBSMainWindow::OBSInit()
{
	if (!obs_startup("zh-CN", nullptr, nullptr)) {
		QMessageBox::warning(nullptr, String("错误"), String("启动OBS失败"));
		return;
	}
	base_set_log_handler(obs_log, nullptr);
	//------------视频预览---------------//
	//初始化音频
	if (!ResetAudio()) {
		QMessageBox::warning(nullptr, String("错误"), String("初始化音频失败"));
		return;
	}
	//初始化视频
	int ret = ResetVidio();
	if (ret != OBS_VIDEO_SUCCESS) {
		QMessageBox::warning(nullptr, String("错误"), String("初始化视频失败"));
		return;
	}
	//return;
	obs_load_all_modules();		// @xp : 初始化所有的插件动态库

	//添加一个场景
	obs_scene* scence = obs_scene_create("default_sence");
	obs_source *scenceSource = obs_scene_get_source(scence);
	obs_set_output_source(0, scenceSource);

	//添加声音源
	bool hasMicDevice = true, hasOutDevice = true;
	hasMicDevice = HasAudioDevices(INPUT_AUDIO_SOURCE);
	hasOutDevice = HasAudioDevices(OUTPUT_AUDIO_SOURCE);

	ResetAudioDevice(INPUT_AUDIO_SOURCE, INPUT_AUDIO_SOURCE, "default", 3);
	ResetAudioDevice(OUTPUT_AUDIO_SOURCE, OUTPUT_AUDIO_SOURCE, "default", 1);
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
	ovi.output_format = VIDEO_FORMAT_NV12;
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

void OBSMainWindow::PlayVideo()
{
	//添加视频源
	//ui.preview->installEventFilter(this);
	AddVideoSource(FFMPEG_SOURCE);
	obs_display_add_draw_callback(ui.preview->GetDisplay(), OBSMainWindow::DrawMainPreview, this);
}

void OBSMainWindow::DrawBackdrop(float cx, float cy)
{
	//if (!box)
	//	return;

	gs_effect_t    *solid = obs_get_base_effect(OBS_EFFECT_SOLID);
	gs_eparam_t    *color = gs_effect_get_param_by_name(solid, "color");
	gs_technique_t *tech = gs_effect_get_technique(solid, "Solid");

	vec4 colorVal;
	vec4_set(&colorVal, 0.0f, 0.0f, 0.0f, 1.0f);
	gs_effect_set_vec4(color, &colorVal);

	gs_technique_begin(tech);
	gs_technique_begin_pass(tech, 0);
	gs_matrix_push();
	gs_matrix_identity();
	gs_matrix_scale3f(float(cx), float(cy), 1.0f);

	//gs_load_vertexbuffer(box);
	gs_draw(GS_TRISTRIP, 0, 0);

	gs_matrix_pop();
	gs_technique_end_pass(tech);
	gs_technique_end(tech);

	gs_load_vertexbuffer(nullptr);
}

void OBSMainWindow::DrawMainPreview(void *data, uint32_t cx, uint32_t cy)
{
	//obs_video_info ovi;
	//obs_get_video_info(&ovi);

	//gs_viewport_push();
	//gs_projection_push();

	///* --------------------------------------- */

	//QSize previewSize(cx, cy);

	//gs_ortho(0.0f, float(ovi.base_width), 0.0f, float(ovi.base_height), -100.0f, 100.0f);
	//gs_set_viewport(0, 0, previewSize.width(), previewSize.height());

	//gs_effect_t *solid = obs_get_base_effect(OBS_EFFECT_SOLID);
	//gs_eparam_t *color = gs_effect_get_param_by_name(solid, "color");
	//gs_technique_t *tech = gs_effect_get_technique(solid, "Solid");

	////begin draw background
	//gs_technique_begin(tech);
	//gs_technique_begin_pass(tech, 0);
	//gs_matrix_push();

	//gs_matrix_identity();
	//gs_matrix_scale3f(viedo_quality_levels[2].output_width, viedo_quality_levels[2].output_height, 1.0f);

	//vec4 backgroundColorVal;
	////vec4_set(&backgroundColorVal, 0.1678f, 0.1678f, 0.1678f, 1.0f);
	//vec4_set(&backgroundColorVal, 0.0f, 0.0f, 0.0f, 1.0f);
	//gs_effect_set_vec4(color, &backgroundColorVal);

	////gs_load_vertexbuffer(obsApp->mBox);
	//gs_draw(GS_TRISTRIP, 0, 0);
	////end draw background

	//gs_matrix_pop();
	//gs_technique_end_pass(tech);
	//gs_technique_end(tech);

	////end draw background

	//gs_set_viewport(0, 0, previewSize.width(), previewSize.height());

	//gs_load_vertexbuffer(nullptr);
	//obs_render_main_view();
	//gs_load_vertexbuffer(nullptr);

	///* --------------------------------------- */
	//gs_matrix_push();
	//gs_matrix_translate3f(0, 0, 0);

	//gs_ortho(0, previewSize.width(), 0, previewSize.height(), -100.0f, 100.0f);
	//gs_reset_viewport();

	//gs_matrix_pop();

	///* --------------------------------------- */

	//gs_projection_pop();
	OBSMainWindow *window = static_cast<OBSMainWindow*>(data);
	obs_video_info ovi;

	obs_get_video_info(&ovi);

	QSize previewSize(cx, cy);
	//window->previewCX = int(window->previewScale * float(ovi.base_width));
	//window->previewCY = int(window->previewScale * float(ovi.base_height));

	gs_viewport_push();
	gs_projection_push();

	/* --------------------------------------- */

	gs_ortho(0.0f, float(ovi.base_width), 0.0f, float(ovi.base_height),
		-100.0f, 100.0f);
	gs_set_viewport(0, 0,
		previewSize.width(), previewSize.height());

	window->DrawBackdrop(float(ovi.base_width), float(ovi.base_height));

	obs_source* scene_source = obs_get_source_by_name("default_sence");
	obs_scene* scene = obs_scene_from_source(scene_source);
	obs_source_t *source = obs_scene_get_source(scene);
	if (source)
		obs_source_video_render(source);
	//else {
	//	obs_render_main_texture();
	//}
	gs_load_vertexbuffer(nullptr);

	/* --------------------------------------- */
	//float right = float(previewSize.width()) - window->previewX;
	//float bottom = float(previewSize.height()) - window->previewY;

	gs_ortho(0, previewSize.width(),0, previewSize.height(),-100.0f, 100.0f);
	gs_reset_viewport();

	//window->ui->preview->DrawSceneEditing();

	/* --------------------------------------- */

	gs_projection_pop();
	gs_viewport_pop();

	UNUSED_PARAMETER(cx);
	UNUSED_PARAMETER(cy);
}
