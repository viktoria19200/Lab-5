#include <iostream>
#include <json/json.h>
#include <string>
#include <winsock2.h>

struct Person {
    std::string name;
    int age;
    std::string email;
};

// Серіалізація структури в JSON
std::string serializePerson(const Person& person) {
    Json::Value root;
    root["name"] = person.name;
    root["age"] = person.age;
    root["email"] = person.email;

    Json::StreamWriterBuilder builder;
    return Json::writeString(builder, root);
}

// Десеріалізація JSON в структуру
Person deserializePerson(const std::string& jsonStr) {
    Json::Value root;
    Json::Reader reader;
    Person person;

    if (reader.parse(jsonStr, root)) {
        person.name = root["name"].asString();
        person.age = root["age"].asInt();
        person.email = root["email"].asString();
    }

    return person;
}

// Приклад надсилання структури через сокет
void sendPersonOverSocket(SOCKET socket, const Person& person) {
    std::string jsonStr = serializePerson(person);
    send(socket, jsonStr.c_str(), jsonStr.length(), 0);
}

// Приклад отримання структури через сокет
Person receivePersonFromSocket(SOCKET socket) {
    char buffer[1024];
    int bytesReceived = recv(socket, buffer, sizeof(buffer), 0);
    buffer[bytesReceived] = '\0';

    return deserializePerson(std::string(buffer));
}

int main()
{
    // Демонстрація використання
    Person p1 = { "John Doe", 30, "john@example.com" };
    std::string serialized = serializePerson(p1);
    std::cout << "Serialized: " << serialized << std::endl;

    Person p2 = deserializePerson(serialized);
    std::cout << "Deserialized: "
        << p2.name << ", "
        << p2.age << ", "
        << p2.email << std::endl;

    return 0;
}