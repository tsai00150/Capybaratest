// NOTES
// UNITTEST MUST BE C++17 to run filesystem
//
#include "../Capybara/intermediary.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <nlohmann/json.hpp>
#include <tuple>
#include <filesystem>           // Test if a File Exists
#include <iostream>

class MockJsonValue {
public:
	Json::Value location;
	Json::Value other;
	Json::Value data;
    string dataString;
	MockJsonValue() {
        location["latitude"] = 3.222;
        location["longitude"] = 78.43;
        other["streetAddress"] = "NYC";

        data["id"] = 3;
        data["doctorName"] = "Capybara";
        data["rating"] = 3.5;
        data["ratingSubmissions"] = 99;
        data["location"] = location;
        data["practiceKeywords"] = Json::arrayValue;
        data["practiceKeywords"].append("Ear");
        data["practiceKeywords"].append("Nose");
        data["practiceKeywords"].append("Throat");
        data["languagesSpoken"] = Json::arrayValue;
        data["languagesSpoken"].append("English");
        data["insurance"] = Json::arrayValue;
        data["languagesSpoken"].append("Aetna");
        data["other"] = other;

        Json::StreamWriterBuilder builder;
        builder["indentation"] = "";
        dataString = Json::writeString(builder, data);
	}

};

TEST(Constructor, preexistingDB) {
    MockJsonValue m;
    Intermediary a;
    
    std::filesystem::path fpath = "db.db";
    EXPECT_TRUE(std::filesystem::exists(fpath));

    // NEED TO CHECK IF this->db already has existing database
}

//TEST(Constructor, newDB) {
//    MockJsonValue m;
//    Intermediary a;
//
//    std::filesystem::path fpath = "db.db";
//    EXPECT_TRUE(std::filesystem::exists(fpath));
//}


TEST(DoctorInfo, getDataByIdSuccess) {
    MockJsonValue m;
    Intermediary a;

    Json::StreamWriterBuilder builder;
    builder["indentation"] = "";
    string dataString = Json::writeString(builder, m.data);
    tuple <int, string> m_tup =  make_tuple(200, dataString);

    EXPECT_EQ(a.doctorInfo("1"), m_tup);
}

//TEST(DoctorInfo, getDataByIdFailure) {
//    MockJsonValue m;
//    EXPECT_EQ(getDataById("s"), NULL);
//}
//
//TEST(DoctorInfo, doctorInfoSuccess200) {
//    MockJsonValue m;
//    tuple <int, string> result = doctorInfo("1");
//
//    EXPECT_EQ(get<0>(result), 200);
//    EXPECT_EQ(get<1>(result), m.dataString);
//}
//
//TEST(DoctorInfo, doctorInfoFailure400) {
//    MockJsonValue m;
//    tuple <int, string> result = doctorInfo("s");
//
//    EXPECT_EQ(get<0>(result), 400);
//    EXPECT_EQ(get<1>(result), "{\"error\": \"Illegal 'id' field!\"}");
//}
//
//TEST(Update, updateUpdateDatabaseSuccess200) {
//	nlohmann::json parsonJson = nlohmann::json::parse(R"(
//    {
//        "id": 3,
//        "fieldToUpdate": "doctorName",
//        "fieldValue": "Capybara Elite"
//    } 
//    )");
//	std::tuple<int, string> result = update(parsonJson);
//	
//    EXPECT_EQ(get<0>(result), 200);
//    EXPECT_EQ(get<1>(result), "");
//}
//
//TEST(Update, updateUpdateDatabaseFailure200) {
//    nlohmann::json parsonJson = nlohmann::json::parse(R"(
//    {
//        "id": 3,
//        "fieldValue": "Capybara Elite"
//    } 
//    )");
//    std::tuple<int, string> result = update(parsonJson);
//
//    EXPECT_EQ(get<0>(result), 400);
//    EXPECT_EQ(get<1>(result), "{\"error\": \"Invalid JSON format in the request body\"}");
//}
//
//TEST(Update, updateCreateDatabaseSuccess200) {
//    nlohmann::json parsonJson = nlohmann::json::parse(R"(
//    {
//        "fieldToUpdate": "doctorName",
//        "fieldValue": "Capybara Elite"
//    } 
//    )");
//    std::tuple<int, string> result = update(parsonJson);
//
//    EXPECT_EQ(get<0>(result), 200);
//    EXPECT_EQ(get<1>(result), "{\"id\": 1}");
//}
//
//TEST(Update, updateCreateDatabaseFailure400) {
//    nlohmann::json parsonJson = nlohmann::json::parse(R"(
//    {
//        "fieldToUpdate": "doctorName"
//    } 
//    )");
//    std::tuple<int, string> result = update(parsonJson);
//
//    EXPECT_EQ(get<0>(result), 400);
//    EXPECT_EQ(get<1>(result), "{\"error\": \"Invalid JSON format in the request body\"}");
//}