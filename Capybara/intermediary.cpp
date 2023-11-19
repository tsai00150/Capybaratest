#include "intermediary.h"

using Record = std::vector<std::string>;
using Records = std::vector<Record>;

Intermediary::Intermediary(DatabaseAbstract* iv) : iv_(iv) {
    // initalizes Database fields
    char* error;
    sqlite3_open("db.db", &this->db);
    int rc = sqlite3_exec(this->db, "CREATE TABLE IF NOT EXISTS doctorInfo(\
                            id INT, \
                            doctorName varchar(100), \
                            rating decimal(18,4), \
                            ratingSubmissions INT, \
                            latitude decimal(18,4), \
                            longitude decimal(18,4), \
                            practiceKeywords varchar(255), \
                            languagesSpoken varchar(255), \
                            insurance varchar(255), \
                            streetAddress varchar(255) \
                            );", NULL, NULL, &error);
    if (rc != SQLITE_OK) {
        cout << "[-] Error Initializing Database!" << endl;
        cerr << error << endl;
    }
}

Intermediary::~Intermediary() {
    sqlite3_close(this->db);
    cout << "[+] Successfully Closed Database!" << endl;
}

tuple <int, string> Intermediary::doctorInfo(const string id) {
    auto data = iv_->getDataById(id);
    if (data == NULL) {
        return make_tuple(400, "{\"error\": \"Illegal 'id' field!\"}");
    }
    else {
        Json::StreamWriterBuilder builder;
        builder["indentation"] = "";
        string dataString = Json::writeString(builder, data);
        return make_tuple(200, dataString);
    }
}

tuple <int, string> Intermediary::update(const nlohmann::json parsedJson) {
    if (parsedJson.find("id") != parsedJson.end() &&
        parsedJson.find("fieldToUpdate") != parsedJson.end() &&
        parsedJson.find("fieldValue") != parsedJson.end()) {
        // id field exists, so we update the existing record

        int doctorId = parsedJson["id"].get<int>();
        string fieldValue = parsedJson["fieldValue"].get<string>();
        string fieldToUpdate = parsedJson["fieldToUpdate"].get<string>();

        int result = iv_->updateDoctorDatabase(to_string(doctorId), fieldToUpdate, fieldValue);
        if (result == 0) {
            return make_tuple(200, "");
        }
        else {
            return make_tuple(400, "{\"error\": \"Unknown error occurred\"}");
        }

    }
    else if (parsedJson.find("fieldToUpdate") != parsedJson.end() &&
        parsedJson.find("fieldValue") != parsedJson.end()) {
        // id field does not exist, so we create a new record
        string fieldValue = parsedJson["fieldValue"].get<string>();
        string fieldToUpdate = parsedJson["fieldToUpdate"].get<string>();
        string id = to_string(iv_->updateCreateNewRecord(fieldToUpdate, fieldValue));
        return make_tuple(200, "{\"id\": " + id + "}");
    }
    else {
        return make_tuple(400, "{\"error\": \"Invalid JSON format in the request body\"}");
    }

}

tuple<int, string> Intermediary::query(string field, string value) {
    // TODO - Seperate query from database
    char* error;
    Records records;
    string query;
    if (field == "rating" || field == "ratingSubmissions") {
        query = "select * from doctorInfo order by " + field + " desc;";
    }
    else if (field == "location") {

        vector<string> v = iv_->split(value, "_");
        if (v.size() != 2) { return make_tuple(400, "{\"error\": \"Wrong location format.\"}"); }
        try {
            float a = stof(v[0]);
            float b = stof(v[1]);
        }
        catch (...) {
            return make_tuple(400, "{\"error\": \"Wrong location format.\"}");
        }
        query = "select id, doctorName, rating, ratingSubmissions, \
latitude, longitude, practiceKeywords, languagesSpoken, \
insurance, streetAddress, \
(latitude-" + v[0] + ")*(latitude-" + v[0] + ")+(longitude-" + v[1] + ")*(longitude-" + v[1] + ") as diff \
from doctorInfo where latitude is not NULL and longitude is not NULL order by diff asc;";

    }

    int exec1 = sqlite3_exec(this->db, query.c_str(), this->select_callback, &records, &error);
    if (exec1 != SQLITE_OK) { return make_tuple(400, "{\"error\": \"Unknown error occurred\"}"); }
    if (records.size() == 0) { return make_tuple(400, "{\"error\": \"No available doctors or wrong request format\"}"); }
    Json::Value location;
    location["latitude"] = records[0][4];
    location["longitude"] = records[0][5];
    Json::Value other;
    other["streetAddress"] = records[0][9];

    Json::Value data;
    if (records[0][0] == "NULL") {
        data["id"] = "NULL";
    }
    else {
        data["id"] = stoi(records[0][0]);
    }

    data["doctorName"] = records[0][1];

    if (records[0][2] == "NULL") {
        data["rating"] = "NULL";
    }
    else {
        data["rating"] = stoi(records[0][2]);
    }
    if (records[0][3] == "NULL") {
        data["ratingSubmissions"] = "NULL";
    }
    else {
        data["ratingSubmissions"] = stoi(records[0][3]);
    }
    data["location"] = location;
    data["practiceKeywords"] = records[0][6];
    data["languagesSpoken"] = records[0][7];
    data["insurance"] = records[0][8];
    data["other"] = other;
    Json::StreamWriterBuilder builder;
    builder["indentation"] = "";
    string dataString = Json::writeString(builder, data);
    return make_tuple(200, dataString);
}

int Intermediary::select_callback(void* ptr, int argc, char* argv[], char* cols[])
// https://stackoverflow.com/questions/15836253/c-populate-vector-from-sqlite3-callback-function
{
    Records* table = static_cast<Records*>(ptr);
    vector<string> row;
    for (int i = 0; i < argc; i++)
        row.push_back(argv[i] ? argv[i] : "NULL");
    table->push_back(row);
    return 0;
}

