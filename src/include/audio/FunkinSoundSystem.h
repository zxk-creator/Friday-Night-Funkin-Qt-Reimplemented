// 为了使用stb_vorbis的ogg解码播放功能
#pragma once
#include "SoundEnum.h"
#include "miniaudio/miniaudio.h"

// 前向声明防止循环依赖
class FunkinSound;

class FunkinSoundSystem
{
	// 必须先初始化Save，再初始化子系统，否则崩溃！！！！！
private:
	float uiSoundVolume = 1;
	float vocalSoundVolume = 1;
	float instSoundVolume = 1;
	friend class GlobalSystemUtils;

	// 短促音效常住内存，减少播放和解码开销
	FunkinSound* confirmSound = nullptr;
	FunkinSound* backSound = nullptr;
	FunkinSound* scrollSound = nullptr;
	// 主菜单音效
	FunkinSound* titleTheme = nullptr;

	// 常用音频组，方便后期设置调音量
	ma_sound_group uiSoundGroup;
	ma_sound_group vocalSoundGroup;
	ma_sound_group instSoundGroup;

	// 核心声音引擎
	ma_engine engine;

public:
	FunkinSoundSystem();

	// 禁止删除，关游戏自动删除，删了就崩溃
protected:
	~FunkinSoundSystem() = default;

	// 我相信你无法调用这个函数，只能通过我的工具类，而工具类已经Clamp过newVolume音量到一个合法的值了，所以我就不Clamp了
	void setSoundVolume(float newVolume,ESoundType soundType);
public:
	void playBuildInSound(EDefaultSoundType soundType);
	void initBuildInSounds();
};