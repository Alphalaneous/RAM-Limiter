#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include "psapi.h"

using namespace geode::prelude;

int ramLimit = 0;

class $modify(MyPlayLayer, PlayLayer) {

    struct Fields {
        int m_initialRam = 0;
    };

    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) {
            return false;
        }
        if (Mod::get()->getSettingValue<bool>("enable-ram-limit")){

            ramLimit = Mod::get()->getSettingValue<int64_t>("ram-limit");

            MEMORYSTATUSEX memInfo;
            memInfo.dwLength = sizeof(MEMORYSTATUSEX);
            GlobalMemoryStatusEx(&memInfo);

            PROCESS_MEMORY_COUNTERS_EX pmc;
            GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));

            m_fields->m_initialRam = pmc.WorkingSetSize/1048576;
            
            schedule(schedule_selector(MyPlayLayer::checkRam));
        }

        return true;
    }

    void checkRam(float dt){
        MEMORYSTATUSEX memInfo;
        memInfo.dwLength = sizeof(MEMORYSTATUSEX);
        GlobalMemoryStatusEx(&memInfo);

        PROCESS_MEMORY_COUNTERS_EX pmc;
        GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));

        int usedMemory = pmc.WorkingSetSize/1048576;
        int levelMemory = usedMemory - m_fields->m_initialRam;

        if (levelMemory > ramLimit) {
            onQuit();
        }
    }
};