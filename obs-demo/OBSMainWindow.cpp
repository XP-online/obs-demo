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
void AddVideoSource(QString filePath) {
	obs_source* source = obs_get_source_by_name(FFMPEG_SOURCE);
	if (!source) {
		OBSData settings = obs_get_source_defaults(FFMPEG_SOURCE);
		obs_data_set_bool(settings, "is_local_file", true);
		obs_data_set_string(settings, "local_file", filePath.toUtf8());
		OBSSource obsSource = obs_source_create(FFMPEG_SOURCE, String("预览窗口").toUtf8(), settings, nullptr);
		obs_source* scene_source = obs_get_source_by_name("default_sence");
		obs_scene* scene = obs_scene_from_source(scene_source);
		obs_scene_atomic_update(scene, add_source_callback, obsSource);
	} else {
		obs_source_release(source);
	}
}
//添加录屏源
void AddCaptureSource() {
	//创建屏幕捕捉资源
	OBSSource source = obs_source_create(MONITOR_CAPTURE, String("预览窗口").toUtf8(), nullptr, nullptr);
	
	if (source) {
		//添加到指定场景中
		obs_source* scene_source = obs_get_source_by_name("default_sence");
		obs_scene* scene = obs_scene_from_source(scene_source);
		obs_sceneitem_t *sceneitem = obs_scene_add(scene, source);
		obs_sceneitem_set_visible(sceneitem, true);
		//设置源的尺寸
		obs_video_info ovi;
		obs_get_video_info(&ovi);

		obs_transform_info itemInfo;
		vec2_set(&itemInfo.pos, 400.0f, 0.0f);
		vec2_set(&itemInfo.scale, 1.0f, 1.0f);
		itemInfo.rot = 0.0f;

		vec2_set(&itemInfo.bounds, /*float(ovi.base_width)*/400.0, 300.0);
		itemInfo.bounds_type = OBS_BOUNDS_SCALE_INNER;
		itemInfo.alignment = OBS_ALIGN_LEFT | OBS_ALIGN_TOP;
		//itemInfo.bounds_alignment = OBS_ALIGN_CENTER;
		obs_sceneitem_set_info(sceneitem, &itemInfo);
		//设置源的尺寸自动缩放
		//set_sceneitem_size(sceneitem);
		//通过source获取属性
		OBSData settings = obs_source_get_settings(source);
		//obs_data_set_bool(settings, "compatibility", true);
		obs_data_set_bool(settings, "capture_cursor", true);
		//更新设置，只有更新，底层才会生效
		obs_source_update(source, settings);
		//obs_scene_atomic_update(scene, add_source_callback, source);
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

void OBSMainWindow::PlayVideo(QString filePath/*= ""*/)
{
	//添加视频源
	AddVideoSource(filePath);
	//添加录屏源
	//AddCaptureSource();
	obs_display_add_draw_callback(ui.preview->GetDisplay(), OBSMainWindow::DrawMainPreview, this);
	//开始推流	   "rtmp://192.168.3.33:1935/live"
	StartStreaming("rtmp://192.168.3.33:1935/live", "tuiliu");
}

void OBSMainWindow::StartStreaming(QString url, QString key)
{
	//添加rtmp-service源设置推流地址
	
	OBSOutput streamOutput = obs_output_create("rtmp_output", "simple_stream", nullptr, nullptr);
	if (streamOutput)
	{
		obs_output_release(streamOutput);
	//	obs_output_update(streamOutput, settings);
	}
	OBSData settings = obs_data_create();

	obs_data_set_string(settings, "server", url.toUtf8());
	obs_data_set_string(settings, "key", key.toUtf8());
	//设置编码器
	OBSEncoder vencoder = obs_video_encoder_create("obs_x264",
		"simple_h264_stream", nullptr, nullptr);
	OBSEncoder aencoder = obs_audio_encoder_create("ffmpeg_aac",
		"simple_aac", nullptr, 0, nullptr);
	OBSService service = obs_service_create(RTMP_CUSTOM,
		"default_service", settings, nullptr);

	obs_output_set_video_encoder(streamOutput, vencoder);
	obs_output_set_audio_encoder(streamOutput, aencoder, 0);
	obs_output_set_service(streamOutput, service);

	obs_encoder_release(vencoder);
	obs_encoder_release(aencoder);
	obs_service_release(service);

	OBSData h264Settings = obs_data_create();
	OBSData aacSettings = obs_data_create();
	
	obs_data_release(h264Settings);
	obs_data_release(aacSettings);

	obs_data_set_int(h264Settings, "bitrate", viedo_quality_levels[2].video_bitrate);
	obs_data_set_int(h264Settings, "keyint_sec", 5);
	obs_data_set_string(h264Settings, "preset", "veryfast");

	//obs_data_set_string(h264Settings, "target_usage", "quality");
	//obs_data_set_string(h264Settings, "rate_control", "VBR");

	obs_data_set_bool(h264Settings, "cbr", false);//固定码率
	obs_data_set_bool(h264Settings, "vfr", true);//动态码率
	obs_data_set_bool(h264Settings, "use_bufsize", true);
	obs_data_set_int(h264Settings, "crf", 23);//画质质量，0-51,0无损 51最次 默认值23， 通常取值范围：[18-28] 每+6，比特率减半  每-6，比特率翻倍

	obs_data_set_bool(aacSettings, "cbr", true);
	obs_data_set_int(aacSettings, "bitrate", 96);

	obs_encoder_update(vencoder, h264Settings);
	obs_encoder_update(aencoder, aacSettings);

	obs_encoder_set_video(vencoder, obs_get_video());
	obs_encoder_set_audio(aencoder, obs_get_audio());

	video_t *video = obs_get_video();
	enum video_format format = video_output_get_format(video);

	if (format != VIDEO_FORMAT_NV12 && format != VIDEO_FORMAT_I420)
		obs_encoder_set_preferred_video_format(vencoder, VIDEO_FORMAT_NV12);

	obs_data_t *outputSettings = obs_data_create();
	obs_data_set_string(outputSettings, "bind_ip", "default");
	obs_data_set_bool(outputSettings, "new_socket_loop_enabled",
		false);
	obs_data_set_bool(outputSettings, "low_latency_mode_enabled",
		false);
	obs_output_update(streamOutput, outputSettings);
	//obs_data_release(outputSettings);

	int retryDelay = 5;
	int maxRetries = 10000;

	obs_output_set_reconnect_settings(streamOutput, maxRetries, retryDelay);

	obs_output_set_delay(streamOutput, 0, OBS_OUTPUT_DELAY_PRESERVE);
	bool isture = obs_output_start(streamOutput);
	bool n = isture;
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
