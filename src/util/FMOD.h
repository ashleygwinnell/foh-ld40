
#ifndef ARK2D_FMOD_H_
#define ARK2D_FMOD_H_

#include "../ARK.h"

#if defined(ARK2D_MACINTOSH) 
	#include "fmod_osx/fmod.hpp"
#elif defined(ARK2D_IPHONE)
	#include "fmod_ios/fmod.hpp"
#elif defined(ARK2D_ANDROID)
	#include "fmod_android/fmod.hpp"
#elif defined(ARK2D_WINDOWS)
	#include "fmod_windows/fmod.hpp"
#elif defined(ARK2D_WINDOWS_PHONE_8)
	#include "fmod_wp8/fmod.hpp"
#elif defined(ARK2D_EMSCRIPTEN_JS)
	#include "fmod_html5/fmod.hpp"
#endif

#include <string>
#include <vector>
using namespace std;

class ArkFMOD {
	public:
		FMOD::System*			m_system;
		FMOD::ChannelGroup* 	m_mastergroup;
    	FMOD_RESULT				m_result;
    	unsigned int 			m_version;
    	void* 					m_extradriverdata;

    	FMOD::DSP* 				m_defaultDSP;
    	FMOD::DSP* 				m_lowpassDSP;
    	FMOD::DSP* 				m_highpassDSP;
    	FMOD::DSP* 				m_flangeDSP;
    	FMOD::DSP* 				m_fftDSP;
    	unsigned int 			m_fftDSP_windowSize;

	public: 
 		ArkFMOD(); 
 		void init();
 		void update();
		virtual ~ArkFMOD();
};

class ArkFMODSound {
	public:
		ArkFMOD*		m_fmod;
		FMOD::Sound* 	m_sound;
		FMOD::Channel* 	m_channel;
		FMOD_RESULT		m_result;
		string 			m_fname;
		float 			m_commonAmplitude;

		float m_offset;

		bool m_usingLowpass;
		float m_usingLowpassFrequency;

		bool m_usingHighpass;
		float m_usingHighpassFrequency;

		float* m_fftdata;
		float* m_fftspec;
		vector<float*> m_fftspecavgs;

		float m_volume;

	public:
		ArkFMODSound(ArkFMOD* arkfmod, string fname);
		void setLowpass(bool b);
		void setLowpassFrequency(float f);

		void setHighpass(bool b);
		void setHighpassFrequency(float f);

		void setFlange(bool b);
		void setFlangeValue(float f);

		void setVolume(float f);
		float getVolume();
		void update();

		void setOffset(float f);
		void play();
		void pause();
		void loop();
		void stop();
		float getCommonAmplitude(float lowcutoff = 0.0f, float highcutoff = 10000.0f);
		bool isPlaying();
		virtual ~ArkFMODSound();
};

#endif /* ARK2D_FMOD_H_ */
