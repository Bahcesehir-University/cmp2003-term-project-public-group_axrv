#pragma once
#include <string>
#include <vector>
#include <unordered_map>

struct ZoneCount {
    std::string zone;
    int count;
};

struct SlotCount {
    std::string zone;
    int hour;
    int count;
};

class TripAnalyzer {
public:
    void ingestFile(const std::string& csvPath);
    std::vector<ZoneCount> topZones(int k = 10) const;
    std::vector<SlotCount> topBusySlots(int k = 10) const;

private:
    std::unordered_map<std::string, int> zoneTripCounts;
    std::unordered_map<std::string, std::unordered_map<int, int>> zoneHourlyCounts;

    void processLine(const std::string& line);
};
