#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

using std::ifstream;
using std::string;
using std::vector;
using std::cout;
using std::endl;

enum class JsonType {
    JsonNull, JsonBoolean, JsonString,
    JsonNumber, JsonObject, JsonArray
};

class JsonValue {
public:
    virtual ~JsonValue() = default;
    virtual JsonType GetType() const = 0;
};

class JsonNull : public JsonValue {
public:
    JsonType GetType() const override { return JsonType::JsonNull; }
};

class JsonBoolean : public JsonValue {
public:
    JsonBoolean(bool value)
        : value_(value) {}

    bool GetValue() const { return value_; }
    JsonType GetType() const override { return JsonType::JsonBoolean; }

private:
    bool value_;
};

class JsonString : public JsonValue {
public:
    JsonString(string value)
        : value_(std::move(value)) {}

    const string& GetValue() const { return value_; }
    JsonType GetType() const override { return JsonType::JsonString; }

private:
    string value_;
};

class JsonNumber : public JsonValue {
public:
    JsonNumber(double value)
        : value_(value) {}

    double GetValue() const { return value_; }
    JsonType GetType() const override { return JsonType::JsonNumber; }

private:
    double value_;
};

class JsonObject : public JsonValue {
public:
    void AddMember(string key, JsonValue* value) {
        members_.push_back({ std::move(key), value });
    }

    JsonType GetType() const override { return JsonType::JsonObject; }
    const vector<std::pair<string, JsonValue*>>& GetMembers() const { return members_; }

private:
    vector<std::pair<string, JsonValue*>> members_;
};

class JsonArray : public JsonValue {
public:
    void AddElement(JsonValue* element) {
        elements_.push_back(element);
    }

    JsonType GetType() const override { return JsonType::JsonArray; }
    const vector<JsonValue*>& GetElements() const { return elements_; }

private:
    vector<JsonValue*> elements_;
};

class JsonParser {
public:
    JsonValue* Parse(const string& input) {
        pos_ = 0;
        input_ = input;
        return ParseValue();
    }
    int count=0;
    int last=0;
    void print(JsonValue* json){

        //Null
        if(static_cast<int>(json->GetType()) == 0){
            cout<<"Null"<<endl;
            return ;
        }
        //布尔值
        else if(static_cast<int>(json->GetType()) == 1){
            cout<<dynamic_cast<JsonBoolean*>(json)->GetValue()<<endl;
            return;
        }
        //字符串
        else if(static_cast<int>(json->GetType()) == 2){
            cout<<dynamic_cast<JsonString*>(json)->GetValue()<<endl;
            return;
        }
        //数字
        else if(static_cast<int>(json->GetType()) == 3){
            cout<<dynamic_cast<JsonNumber*>(json)->GetValue()<<endl;
            return;
        }
        //对象
        else if(static_cast<int>(json->GetType()) == 4){
            cout<<"{"<<endl;
            const vector<std::pair<string, JsonValue*>>& members = static_cast<JsonObject*>(json)->GetMembers();
            count++;
            for (const auto& member : members) {
                const string& key = member.first;
                if(count>1){
                    for (int i = 0; i < last+4; ++i) {
                        cout<<" ";
                    }
                }
                if(count==1) {
                    last = key.size();
                }
                JsonValue* value = member.second;
                for (int i = 0; i < count; ++i) {
                    cout <<" ";
                }
                cout <<  key << " : " ;
                print(member.second);
            }
            for (int i = 0; i < count+last+2; ++i) {
                cout <<" ";
            }
            cout<<"}"<<endl;
            count=0;
            return ;
        }
        //数组
        else if(static_cast<int>(json->GetType()) == 5){
            cout<<"["<<endl;
            const vector<JsonValue*>& elements = static_cast<JsonArray*>(json)->GetElements();
            for (const auto& element : elements) {
                print(element);
            }
            cout<<"]"<<endl;
            return;
        }
    }

private:
    JsonValue* ParseValue() {
        if (pos_ >= input_.size()) return nullptr;

        char c = input_[pos_];
        if (c == 'n') {
            return ParseNull();
        }
//        else if(c=='/'){
//            ParseVaccum();
//        }
//        else if(c==' '){
//            Parsekong();
//        }
        else if (c == 't' || c == 'f') {
            return ParseBoolean();
        }
        else if (isdigit(c) || c == '-') {
            return ParseNumber();
        }
        else if (c == '"') {
            return ParseString();
        }
        else if (c == '{') {
            return ParseObject();
        }
        else if (c == '[') {
            return ParseArray();
        }

        return nullptr;
    }

    JsonNull* ParseNull() {
        Expect("null");
        return new JsonNull();
    }
    void Parsekong(){
        Expect(" ");
    }
    void ParseVaccum(){
        Expect("/n");
    }
    JsonBoolean* ParseBoolean() {
        bool value = false;
        if (Consume("true")) {
            value = true;
        }
        else if (Consume("false")) {
            value = false;
        }
        else {
            return nullptr;
        }

        return new JsonBoolean(value);
    }

    JsonNumber* ParseNumber() {
        string number_str;
        bool is_negative = false;

        if (input_[pos_] == '-') {
            is_negative = true;
            pos_++;
        }

        while (isdigit(input_[pos_])) {
            number_str += input_[pos_++];
        }

        if (input_[pos_] == '.') {
            number_str += input_[pos_++];

            while (isdigit(input_[pos_])) {
                number_str += input_[pos_++];
            }
        }

        if (is_negative) number_str = "-" + number_str;
        return new JsonNumber(stod(number_str));
    }

    JsonString* ParseString() {
        string value;
        pos_++;
        while (pos_ < input_.size()) {
            char c = input_[pos_];
            if (c == '"') {
                pos_++;
                return new JsonString(value);
            }
            else if (c == '\\') {
                pos_++;
                char escaped_char = ParseEscapedChar();
                if (escaped_char == 0) {
                    return nullptr;
                }
                value += escaped_char;
            }
            else {
                pos_++;
                value.push_back(c);
            }
        }

        return nullptr;
    }

    char ParseEscapedChar() {
        char c = input_[pos_];
        pos_++;

        switch (c) {
        case '"': return '"';
        case '\\': return '\\';
        case '/': return '/';
        case 'b': return '\b';
        case 'f': return '\f';
        case 'n': return '\n';
        case 'r': return '\r';
        case 't': return '\t';
        }

        return 0;
    }

    JsonObject* ParseObject() {
        JsonObject* object = new JsonObject();

        Expect("{");

        while (pos_ < input_.size() && input_[pos_] != '}') {
            while(input_[pos_]!='"'){
                pos_++;
            }
            string key = ParseString()->GetValue();
            while(input_[pos_]==' '){
                pos_++;
            }
            Expect(":");
            while(input_[pos_]==' '){
                pos_++;
            }
            JsonValue* value = ParseValue();
            if (key.empty() || value == nullptr) {
                delete object;
                return nullptr;
            }

            object->AddMember(key, value);

            if (input_[pos_] == ',') {
                pos_++;
            }
            else {
                break;
            }
        }
        Expect("}");

        return object;
    }

    JsonArray* ParseArray() {
        JsonArray* array = new JsonArray();

        Expect("[");
        while (pos_ < input_.size() && input_[pos_] != ']') {
            JsonValue* element = ParseValue();

            if (element == nullptr) {
                delete array;
                return nullptr;
            }

            array->AddElement(element);

            if (input_[pos_] == ',') {
                pos_++;
            }
            else {
                break;
            }
        }
        Expect("]");

        return array;
    }

    void Expect(string expected) {
        Consume(expected);
    }

    bool Consume(string expected) {
        if (input_.substr(pos_, expected.size()) == expected) {
            pos_ += expected.size();
            return true;
        }

        return false;
    }

private:
    size_t pos_ = 0;
    string input_;
};

int main() {
    // 从文件中读取JSON字符串
    ifstream input_file("../settings.json");
    string input((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
//    string line;
//    getline(input_file,line);
    input_file.close();

    // 解析JSON字符串
    JsonParser parser;
    JsonValue* json = parser.Parse(input);
    parser.print(json);
    delete json;
    return 0;
}