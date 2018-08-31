

#include "FMOD.h"
#include <ARK2D/UI/+.h>

 FMOD_RESULT F_CALLBACK defaultDSPCallback(FMOD_DSP_STATE* dsp_state, float* inbuffer, float* outbuffer, unsigned int length, int inchannels, int *outchannels) 
{
    FMOD_RESULT result;
    char name[256];
    unsigned int userdata;
    FMOD::DSP *thisdsp = (FMOD::DSP*) dsp_state->instance; 

	// This redundant call just shows using the instance parameter of FMOD_DSP_STATE to 
	// call a DSP information function. 
    result = thisdsp->getInfo(name, 0, 0, 0, 0);
    if (result != FMOD_OK) { 
    	ErrorDialog::createAndShow("could not get dsp info"); 
    	exit(0); 
    }

    result = thisdsp->getUserData((void **)&userdata);
 	if (result != FMOD_OK) { 
 		ErrorDialog::createAndShow("could not get dsp user data"); 
 		exit(0); 
 	}

 	float amp = 0.0f;

    /*
        This loop assumes inchannels = outchannels, which it will be if the DSP is created with '0' 
        as the number of channels in FMOD_DSP_DESCRIPTION.  
        Specifying an actual channel count will mean you have to take care of any number of channels coming in,
        but outputting the number of channels specified. Generally it is best to keep the channel 
        count at 0 for maximum compatibility.
    */
    float total_amp = 0.0f;
    int total_count = 0;
    for (unsigned int samp = 0; samp < length; samp++) 
    { 
		// Feel free to unroll this.
        for (int chan = 0; chan < *outchannels; chan++)
        {
            // This DSP filter just halves the volume! 
			// Input is modified, and sent to output.
            float inn = inbuffer[(samp * inchannels) + chan];
            outbuffer[(samp * *outchannels) + chan] = inn;// * 0.2f;
        }
    } 

    return FMOD_OK; 
} 

ArkFMOD::ArkFMOD()
{
	m_system = NULL;
	m_mastergroup = NULL;
	m_result = FMOD_OK;
	m_version = 0;
	m_extradriverdata = NULL;


}  

void ArkFMOD::init() 
{
	ARK2D::getLog()->v("Initialising FMOD sound engine.");

    // Init FMOD
	m_result = FMOD::System_Create(&m_system);
	if (m_result != FMOD_OK) { 
		ErrorDialog::createAndShow("Could not initialise fmod system."); 
		exit(0); 
	}
	

    m_system->getVersion(&m_version);
   	if (m_result != FMOD_OK) { 
   		ErrorDialog::createAndShow("Could not get fmod version."); 
   		exit(0); 
   	}

    if (m_version < FMOD_VERSION) {
        ErrorDialog::createAndShow("fmod lib and header versions do not match.");
        exit(0);
    }
	
	// android latency thing.
    #ifdef ARK2D_ANDROID
    	m_result = m_system->setOutput(FMOD_OUTPUTTYPE_AUDIOTRACK);
    	if (m_result != FMOD_OK) {
    		ErrorDialog::createAndShow("Could not set fmod system output (audiotrack).");
    		exit(0);
    	}
    #endif

    m_result = m_system->init(32, FMOD_INIT_NORMAL, m_extradriverdata);
    if (m_result != FMOD_OK) { 
    	ErrorDialog::createAndShow("Could not init fmod."); 
    	exit(0); 
    }


	
	#ifdef ARK2D_ANDROID
	   m_system->setDSPBufferSize(2048, 4); // 4096
    #endif

    // Init default DSP.
    { 
        FMOD_DSP_DESCRIPTION dspdesc; 
        memset(&dspdesc, 0, sizeof(dspdesc));
        
        strncpy(dspdesc.name, "My first DSP unit", sizeof(dspdesc.name));
        dspdesc.version = 0x00010000;
        dspdesc.numinputbuffers = 1;
        dspdesc.numoutputbuffers = 1;
        dspdesc.read = defaultDSPCallback; 
        dspdesc.userdata = (void *)0x12345678; 

        m_result = m_system->createDSP(&dspdesc, &m_defaultDSP); 
        if (m_result != FMOD_OK) { ErrorDialog::createAndShow("Could not create dsp."); exit(0); }
    } 


	
    { 
        m_result = m_system->createDSPByType(FMOD_DSP_TYPE_ITLOWPASS, &m_lowpassDSP); 
        if (m_result != FMOD_OK) {
            ErrorDialog::createAndShow("Could not create dsp.");
            exit(0);
        }
    } 


    { 
        m_result = m_system->createDSPByType(FMOD_DSP_TYPE_HIGHPASS, &m_highpassDSP); 
        if (m_result != FMOD_OK) {
            ErrorDialog::createAndShow("Could not create dsp.");
            exit(0);
        }
    }

    { 
        m_result = m_system->createDSPByType(FMOD_DSP_TYPE_TREMOLO, &m_flangeDSP); 
        if (m_result != FMOD_OK) {
			ErrorDialog::createAndShow("Could not create flange dsp.");
			exit(0);
		}
    } 

   	
   	// Attach default DSP.
    m_result = m_defaultDSP->setBypass(false);
    if (m_result != FMOD_OK) { 
    	ErrorDialog::createAndShow("Could not set dsp bypass."); 
    	exit(0); 
    }

    m_result = m_system->getMasterChannelGroup(&m_mastergroup);
    if (m_result != FMOD_OK) { 
    	ErrorDialog::createAndShow("Could not set channel master group."); 
    	exit(0); 
    }
	
	#if defined(ARK2D_WINDOWS_PHONE_8) || defined(ARK2D_WINDOWS) || defined(ARK2D_EMSCRIPTEN_JS) || defined(ARK2D_IOS)
		m_result = m_mastergroup->addDSP(1, m_defaultDSP);
	#else
		m_result = m_mastergroup->addDSP(1, m_defaultDSP, 0);
	#endif
    if (m_result != FMOD_OK) { 
    	ErrorDialog::createAndShow("Could not add dsp to master group."); 
    	exit(0); 
    }

    { 
        m_fftDSP_windowSize = 2048;

        m_result = m_system->createDSPByType(FMOD_DSP_TYPE_FFT, &m_fftDSP); 
        if (m_result != FMOD_OK) { ErrorDialog::createAndShow("Could not create loudness meter dsp."); exit(0); }
		
		m_result = m_fftDSP->setParameterInt(FMOD_DSP_FFT_WINDOWTYPE, FMOD_DSP_FFT_WINDOW_RECT);
		if (m_result != FMOD_OK) { ErrorDialog::createAndShow("Could not set fft dsp window type."); exit(0); }

		m_result = m_fftDSP->setParameterInt(FMOD_DSP_FFT_WINDOWSIZE, m_fftDSP_windowSize);
		if (m_result != FMOD_OK) { ErrorDialog::createAndShow("Could not set fft dsp window size."); exit(0); }

		#if defined(ARK2D_WINDOWS_PHONE_8) || defined(ARK2D_WINDOWS) || defined(ARK2D_EMSCRIPTEN_JS) || defined(ARK2D_IOS)
			m_result = m_mastergroup->addDSP(0, m_fftDSP); // FMOD_CHANNELCONTROL_DSP_HEAD
		#else
			m_result = m_mastergroup->addDSP(0, m_fftDSP, 0); // FMOD_CHANNELCONTROL_DSP_HEAD
		#endif
		if (m_result != FMOD_OK) { ErrorDialog::createAndShow("Could not add fft dsp."); exit(0); }

    }

}

void ArkFMOD::update() 
{
	// set dsp bypass?
	//m_result = m_mydsp->setBypass(!m_usemydsp);
   	//if (m_result != FMOD_OK) { ErrorDialog::createAndShow("Could not set mydsp bypass."); exit(0); }

	// update fmod system
	//ARK2D::getLog()->g("update fmod system");
	m_result = m_system->update();
	if (m_result != FMOD_OK) { 
		ErrorDialog::createAndShow("Could not update fmod system."); 
		exit(0); 
	}

}

ArkFMOD::~ArkFMOD()
{
	 
}


// ------

ArkFMODSound::ArkFMODSound(ArkFMOD* arkfmod, string fname):
	m_fmod(arkfmod),
	m_sound(NULL),
    m_channel(NULL),
	m_fname(""),

    m_offset(0.0f),
	m_usingLowpass(false),
    m_usingLowpassFrequency(5000.0f),
	m_usingHighpass(false),
    m_usingHighpassFrequency(5000.0),
    m_volume(1.0f)
{
	m_result = FMOD_OK;
	
	string name = ARK2D::getContainer()->getResourcePath();//FileUtil::getResourcePath();
	name += fname;
	m_fname = name;

	#if defined(ARK2D_ANDROID) || defined(ARK2D_IOS)
        RawDataReturns* data = Resource::getRawData(m_fname);
        string ext = StringUtil::getExtension(m_fname);


        FMOD_CREATESOUNDEXINFO exinfo;
        memset(&exinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
        exinfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
        exinfo.length = data->size;  
        exinfo.format = FMOD_SOUND_FORMAT_NONE;
        //if (ext == "ogg") {
        //   exinfo.format = FMOD_SOUND_FORMAT_VORBIS;
        //}
        
        m_result = m_fmod->m_system->createSound((const char*) data->data, FMOD_OPENMEMORY | FMOD_LOOP_OFF, &exinfo, &m_sound); // FMOD_SOFTWARE must be used for frequency analysis. // FMOD_DEFAULT 

        delete data;
	#elif defined(ARK2D_WINDOWS_PHONE_8) || defined(ARK2D_WINDOWS) || defined(ARK2D_EMSCRIPTEN_JS)
		
        RawDataReturns* data = Resource::getRawData(fname);
        string ext = StringUtil::getExtension(fname);

        FMOD_CREATESOUNDEXINFO exinfo;
        memset(&exinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
        exinfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
        exinfo.length = data->size;  
        exinfo.format = FMOD_SOUND_FORMAT_NONE;
        if (ext == "ogg") {
           //exinfo.format = FMOD_SOUND_FORMAT_VORBIS;
        }
        
        m_result = m_fmod->m_system->createSound((const char*) data->data, FMOD_OPENMEMORY | FMOD_LOOP_OFF, &exinfo, &m_sound); // FMOD_SOFTWARE must be used for frequency analysis. // FMOD_DEFAULT 

		delete data;
    //#elif defined(ARK2D_IOS)
    //    m_result = m_fmod->m_system->createSound(m_fname.c_str(), FMOD_OPENMEMORY | FMOD_LOOP_OFF, 0, &m_sound); // FMOD_SOFTWARE must be used for frequency analysis. // FMOD_DEFAULT
    
    #else
    	
		m_result = m_fmod->m_system->createSound(m_fname.c_str(), FMOD_SOFTWARE, 0, &m_sound); // FMOD_SOFTWARE must be used for frequency analysis. // FMOD_DEFAULT
        
		

    #endif

    if (m_result != FMOD_OK) { 
        ARK2D::getLog()->e(StringUtil::append("Could not load sound: ",m_fname) ); 
        exit(0); 
    } 

    m_fftspec = new float[m_fmod->m_fftDSP_windowSize];
    //m_fftspecavgs = new float[m_fmod->m_fftDSP_windowSize * 4];
}

void ArkFMODSound::setLowpass(bool b) 
{
	// #ifdef ARK2D_EMSCRIPTEN_JS
 //    return;
 //    #endif
    //if (!m_usingLowpass) {
        m_usingLowpass = true;
        if (m_channel != NULL) { 
    		
    		#if defined(ARK2D_WINDOWS_PHONE_8) || defined(ARK2D_WINDOWS)	|| defined(ARK2D_EMSCRIPTEN_JS)|| defined(ARK2D_IOS)
    			m_result = m_channel->addDSP(0, m_fmod->m_lowpassDSP);
    		#else
    			m_result = m_channel->addDSP(0,  m_fmod->m_lowpassDSP, 0 );
    		#endif
    		if (m_result != FMOD_OK) { 
        		ErrorDialog::createAndShow(StringUtil::append("Could not add lowpass dsp: ", m_fname) ); 
        		exit(0); 
        	}

        	

    	}
   // }
}
void ArkFMODSound::setLowpassFrequency(float f) {
    // #ifdef ARK2D_EMSCRIPTEN_JS
    // return;
    // #endif

    m_usingLowpassFrequency = f;

    if (m_channel != NULL) { 

        
        // FMOD_DSP_ITLOWPASS_CUTOFF,    /* (Type:float) - Lowpass cutoff frequency in hz.  1.0 to 22000.0.  Default = 5000.0/ */
        // FMOD_DSP_ITLOWPASS_RESONANCE  /* (Type:float) - Lowpass resonance Q value.  0.0 to 127.0.  Default = 1.0. */
        m_result = m_fmod->m_lowpassDSP->setParameterFloat(FMOD_DSP_ITLOWPASS_CUTOFF, m_usingLowpassFrequency);
        if (m_result != FMOD_OK) { 
            ErrorDialog::createAndShow(StringUtil::append("Could not set lowpass cutoff: ", m_fname) ); 
            exit(0); 
        }

    }
}
void ArkFMODSound::setHighpass(bool b) {
    // m_usingHighpass = true;
    // if (m_channel != NULL) { 

    //     if (m_usingHighpassFrequency) {
        
    //         #if defined(ARK2D_WINDOWS_PHONE_8) || defined(ARK2D_WINDOWS)    
    //             m_result = m_channel->addDSP(0, m_fmod->m_highpassDSP);
    //         #else
    //             m_result = m_channel->addDSP(0,  m_fmod->m_highpassDSP, 0 );
    //         #endif
    //         if (m_result != FMOD_OK) { 
    //             ErrorDialog::createAndShow(StringUtil::append("Could not add highpass dsp: ", m_fname) ); 
    //             exit(0); 
    //         }
    //     } else {
    //         #if defined(ARK2D_WINDOWS_PHONE_8) || defined(ARK2D_WINDOWS)    
    //             m_result = m_channel->removeDSP(m_fmod->m_highpassDSP);
    //         #else
    //             m_result = m_channel->removeDSP(m_fmod->m_highpassDSP);
    //         #endif
    //         if (m_result != FMOD_OK) { 
    //             ErrorDialog::createAndShow(StringUtil::append("Could not remove flange dsp: ", m_fname) ); 
    //             exit(0); 
    //         }
    //     }
    // }
}
void ArkFMODSound::setHighpassFrequency(float f) {
    // m_usingHighpassFrequency = f;

    // if (m_channel != NULL) { 
    //     // FMOD_DSP_ITLOWPASS_CUTOFF,    /* (Type:float) - Lowpass cutoff frequency in hz.  1.0 to 22000.0.  Default = 5000.0/ */
    //     // FMOD_DSP_ITLOWPASS_RESONANCE  /* (Type:float) - Lowpass resonance Q value.  0.0 to 127.0.  Default = 1.0. */
    //     m_result = m_fmod->m_highpassDSP->setParameterFloat(FMOD_DSP_HIGHPASS_CUTOFF, m_usingHighpassFrequency);
    //     if (m_result != FMOD_OK) { 
    //         ErrorDialog::createAndShow(StringUtil::append("Could not set highpass cutoff: ", m_fname) ); 
    //         exit(0); 
    //     }

    // }
}
void ArkFMODSound::setFlange(bool b) {
    m_usingHighpassFrequency = b;
    if (m_channel != NULL) { 

        if (m_usingHighpassFrequency){        
            #if defined(ARK2D_WINDOWS_PHONE_8) || defined(ARK2D_WINDOWS)    || defined(ARK2D_EMSCRIPTEN_JS)|| defined(ARK2D_IOS)
                m_result = m_channel->addDSP(0, m_fmod->m_flangeDSP);
            #else
                m_result = m_channel->addDSP(0,  m_fmod->m_flangeDSP, 0 );
            #endif
            if (m_result != FMOD_OK) { 
                ErrorDialog::createAndShow(StringUtil::append("Could not add flange dsp: ", m_fname) ); 
                exit(0); 
            }
        } else {
            #if defined(ARK2D_WINDOWS_PHONE_8) || defined(ARK2D_WINDOWS)   || defined(ARK2D_EMSCRIPTEN_JS) 
                m_result = m_channel->removeDSP(m_fmod->m_flangeDSP);
            #else
                m_result = m_channel->removeDSP(m_fmod->m_flangeDSP);
            #endif
            if (m_result != FMOD_OK) { 
                ErrorDialog::createAndShow(StringUtil::append("Could not add flange dsp: ", m_fname) ); 
                exit(0); 
            }
        }
    }
}


void ArkFMODSound::setVolume(float f) {
    // #ifdef ARK2D_EMSCRIPTEN_JS
    // return;
    // #endif

    m_volume = f;
    if (m_channel != NULL) {
        m_channel->setVolume(m_volume);
    }
}
float ArkFMODSound::getVolume() {
    return m_volume;
}
void ArkFMODSound::update() {
	
}

bool ArkFMODSound::isPlaying() 
{
	// #ifdef ARK2D_EMSCRIPTEN_JS
 //    return false;
 //    #endif

    bool b1;
	m_channel->getPaused(&b1);
	if (b1) { return false; }

	bool b;
	m_channel->isPlaying(&b);
	return b;
}
void ArkFMODSound::play() 
{
	// #ifdef ARK2D_EMSCRIPTEN_JS
 //    return;
 //    #endif

    //bool firstplay = false;
    stop();

    m_fmod->m_system->playSound(m_sound, NULL, false, &m_channel);
    m_channel->setVolume(m_volume);
	m_channel->setPaused(false);

    //if (firstplay) {
    //    setLowpass(m_usingLowpass);
    //    setLowpassFrequency(m_usingLowpassFrequency);
    //    setOffset(m_offset);
    //}
}
void ArkFMODSound::setOffset(float f) {
    m_offset = f;
	if (m_channel != NULL) { 
		m_channel->setPosition(int(round(f*1000.0f)), FMOD_TIMEUNIT_MS);
	}
}

void ArkFMODSound::loop() 
{
    m_fmod->m_system->playSound(m_sound, NULL, true, &m_channel);
    m_channel->setLoopCount(-1);
    m_channel->setMode(FMOD_LOOP_NORMAL);
    m_channel->setPosition(0, FMOD_TIMEUNIT_MS); // this flushes the buffer to ensure the loop mode takes effect
}

void ArkFMODSound::stop() 
{
	if (m_channel != NULL) {
		m_result = m_channel->stop();
        if (m_result != FMOD_OK) {
            ARK2D::getLog()->e("Could not stop sound channel.");
        }
	}
}
void ArkFMODSound::pause() 
{
	if (m_channel != NULL) {
		m_result = m_channel->setPaused(true);
        if (m_result != FMOD_OK) {
            ARK2D::getLog()->e("Could not pause sound channel.");
        }
	}
}

float ArkFMODSound::getCommonAmplitude(float lowcutoff, float highcutoff) 
{
    float val = 0;

   /* m_result = m_fmod->m_fftDSP->getParameterFloat(FMOD_DSP_FFT_DOMINANT_FREQ, &val, 0, 0);
    if (m_result != FMOD_OK) { 
        ErrorDialog::createAndShow("Could not get fft dsp parameter dominant freq."); 
        exit(0); 
    }*/

    /*float retval = (val - lowcutoff) / (highcutoff - lowcutoff);
    if (retval < 0.0f) { 
    	retval = 0.0f; 
    } else if (retval > 1.0f) { 
    	retval = 0.0f;
    } 
    return retval;*/

   /* int windowSize = m_fmod->m_fftDSP_windowSize;
    int nyquist = windowSize / 2;

    
    unsigned int len;
    char s[256];

    m_result = m_fmod->m_fftDSP->getParameterData(FMOD_DSP_FFT_SPECTRUM, (void **)&m_fftdata, &len, s, 256);
    if (m_result != FMOD_OK) { 
        ErrorDialog::createAndShow("Could not get fft dsp parameter data."); 
        exit(0); 
    }

    
	for(unsigned int i = 0; i < windowSize; i++) {
		m_fftspec[i] = (m_fftdata[i] + m_fftdata[windowSize+i])/2;
	}

	// normalise?
	auto maxIterator = std::max_element(&m_fftspec[0], &m_fftspec[windowSize]);
	float maxVol = *maxIterator;
 	if (maxVol > 0.0f) {
		for(unsigned int i = 0; i < windowSize; i++) {
			m_fftspec[i] = m_fftspec[i] / maxVol;
		}
	}



	float each = 22050.0f / float(windowSize);
	int lowcutoff_index = round(lowcutoff / each);
	int highcutoff_index = round(highcutoff / each);

	//int domffq_index = round(val / each);
	//return m_fftspec[domffq_index];

	if (lowcutoff_index == highcutoff_index) {
		return m_fftspec[lowcutoff_index];
	}
	ARK2D::getLog()->v(StringUtil::append("low: ", lowcutoff_index));
	ARK2D::getLog()->v(StringUtil::append("high : ", highcutoff_index));
	float total = 0;
	float count = 0;
	for(unsigned int i = lowcutoff_index; i <= highcutoff_index; i++) {
		total += m_fftspec[i];
		count += 1.0f;
	}
	if (total == 0 || count == 0) { return 0; }
	float thing = total / count;
	if (thing > 1.0f) { thing = 1.0f; }
	else if (thing < 0.0f) { thing = 0.0f; }
	return thing;
	*/
    return val;
}

ArkFMODSound::~ArkFMODSound() 
{

}




