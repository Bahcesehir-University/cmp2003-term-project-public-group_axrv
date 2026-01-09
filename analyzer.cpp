#include "analyzer.h"
#include <fstream>
#include <algorithm>
#include <string>

int parseHour(const std::string& timeStr) {
    if (timeStr.length() < 5) return -1;

    char firstDigit = timeStr[0];
    char secondDigit = timeStr[1];

    if (firstDigit < '0' || firstDigit > '9' || secondDigit < '0' || secondDigit > '9') return -1;
    if (timeStr[2] != ':') return -1;

    int hour = (firstDigit - '0') * 10 + (secondDigit - '0');
    if (hour < 0 || hour > 23) return -1;

    return hour;
}

void TripAnalyzer::processLine(const std::string& line) {
    short commas = 0;
    for (char ch : line) {
        if (ch == ',') commas++;
    }
    if (commas < 5) return;

    std::size_t firstCommaPos = line.find(',');
    if (firstCommaPos == std::string::npos) return;

    std::size_t secondCommaPos = line.find(',', firstCommaPos + 1);
    if (secondCommaPos == std::string::npos) return;

    std::size_t thirdCommaPos = line.find(',', secondCommaPos + 1);
    if (thirdCommaPos == std::string::npos) return;

    std::size_t fourthCommaPos = line.find(',', thirdCommaPos + 1);
    if (fourthCommaPos == std::string::npos) return;

    std::string pickupZone = line.substr(firstCommaPos + 1, secondCommaPos - firstCommaPos - 1);
    if (pickupZone.empty()) return;

    std::string pickupTime = line.substr(thirdCommaPos + 1, fourthCommaPos - thirdCommaPos - 1);
    if (pickupTime.empty()) return;

    std::size_t timeSpacePos = pickupTime.find(' ');
    if (timeSpacePos == std::string::npos || timeSpacePos + 3 > pickupTime.length()) {
        return;
    }

    std::string timePortion = pickupTime.substr(timeSpacePos + 1);
    int pickupHour = parseHour(timePortion);
    if (pickupHour < 0) return;

    zoneTripCounts[pickupZone]++;
    zoneHourlyCounts[pickupZone][pickupHour]++;
}

void TripAnalyzer::ingestFile(const std::string& csvPath) {
    std::ifstream file(csvPath);
    
    if (!file.is_open()) return;

    std::string line;

    bool isFirstLine = true;

    while (std::getline(file, line)) {
        if (isFirstLine) {
            isFirstLine = false;
            continue;
        }
        processLine(line);
    }
}

std::vector<ZoneCount> TripAnalyzer::topZones(int k) const {
    std::vector<ZoneCount> topZones;
    
    for (const auto& pair : zoneTripCounts) {
        topZones.push_back({pair.first, pair.second});
    }

    std::sort(topZones.begin(), topZones.end(), [](const ZoneCount& a, const ZoneCount& b) {
        if (a.count != b.count) {
            return a.count > b.count;
        }
        return a.zone < b.zone;
    });

    if (topZones.size() > (size_t)k) {
        topZones.resize(k);
    }
    return topZones;
}

std::vector<SlotCount> TripAnalyzer::topBusySlots(int k) const {
    std::vector<SlotCount> topBusySlots;

    for (const auto& zonePair : zoneHourlyCounts) {
        const std::string& zone = zonePair.first;
        for (const auto& hourPair : zonePair.second) {
            topBusySlots.push_back({zone, hourPair.first, hourPair.second});
        }
    }

    std::sort(topBusySlots.begin(), topBusySlots.end(), [](const SlotCount& a, const SlotCount& b) {
        if (a.count != b.count) {
            return a.count > b.count;
        }
        if (a.zone != b.zone) {
            return a.zone < b.zone;
        }
        return a.hour < b.hour;
    });

    if (topBusySlots.size() > (size_t)k) {
        topBusySlots.resize(k);
    }
    return topBusySlots;
}
