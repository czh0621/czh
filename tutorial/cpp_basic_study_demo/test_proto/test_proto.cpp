/**
 * @file       test_proto.cpp
 * @date       2023/9/13
 * @since      2023-09-13
 * @author     czh
 */

#include "test.pb.h"
#include <google/protobuf/message.h>
#include <google/protobuf/util/json_util.h>
#include <spdlog/spdlog.h>
#include <tuple>
#include <type_traits>


void test_reflect()
{
    std::string message_name    = "tutorial.Person";
    std::string field_name      = "name";
    const auto* descriptor_pool = google::protobuf::DescriptorPool::generated_pool();
    const auto* descriptor      = descriptor_pool->FindMessageTypeByName(message_name);

    if (descriptor) {
        auto* prototype =
            google::protobuf::MessageFactory::generated_factory()->GetPrototype(descriptor);
        if (prototype) {
            auto instance = prototype->New();
            spdlog::info(
                "Message descriptor full_name:{},descriptor name:{},Message instance typename:{}",
                descriptor->full_name(),
                descriptor->name(),
                instance->GetTypeName());
            const auto* des       = instance->GetDescriptor();
            const auto* ref       = instance->GetReflection();
            auto*       field_des = des->FindFieldByName(field_name);
            ref->SetString(const_cast<google::protobuf::Message*>(instance), field_des, "zhangsan");
            // 此处可以使用tuple 存储函数类型来进行转换
            auto* real_instance = dynamic_cast<tutorial::Person*>(instance);
            spdlog::info("person name:{}", real_instance->name());
        }
    }
}

void test_tuple_cast()
{
    std::string message_name    = "tutorial.Person";
    std::string field_name      = "name";
    const auto* descriptor_pool = google::protobuf::DescriptorPool::generated_pool();
    const auto* descriptor      = descriptor_pool->FindMessageTypeByName(message_name);

    if (descriptor) {
        auto* prototype =
            google::protobuf::MessageFactory::generated_factory()->GetPrototype(descriptor);
        if (prototype) {
            auto instance = prototype->New();
            spdlog::info("Message type name:{},Message instance:{}",
                         descriptor->full_name(),
                         instance->GetTypeName());
            const auto* des       = instance->GetDescriptor();
            const auto* ref       = instance->GetReflection();
            auto*       field_des = des->FindFieldByName(field_name);
            ref->SetString(const_cast<google::protobuf::Message*>(instance), field_des, "zhangsan");
            auto* real_instance = dynamic_cast<tutorial::Person*>(instance);
            spdlog::info("person name:{}", real_instance->name());
        }
    }
}
template<typename T>
struct traits
{
    using bare_type = std::tuple<std::remove_const_t<std::remove_reference_t<T>>>;
};


void test_pb_value()
{
    tutorial::Person person;
    if (person.has_name()) {
        spdlog::info("person has name");
    }
    else {
        spdlog::info("person has defualt name");
    }

    person.set_name("lisi");
    if (person.has_name()) {
        spdlog::info("person has name:{}", person.name());
    }
    else {
        spdlog::info("person has default name");
    }
}

std::string base64_decode(const std::string& encoded)
{
    // Base64 字符集
    static const std::string base64_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    std::string decoded;
    int         val = 0, valb = -8;
    for (unsigned char c : encoded) {
        if (c == '=') break;               // '=' 是 Base64 填充字符，结束解码
        auto pos = base64_chars.find(c);   // 查找字符的位置
        if (pos == std::string::npos) {
            throw std::invalid_argument("Input contains invalid Base64 characters.");
        }

        val = (val << 6) + pos;
        if ((valb += 6) >= 0) {
            decoded.push_back(char((val >> valb) & 0xFF));   // 取出解码后的字符
            valb -= 8;
        }
    }

    return decoded;
}

void test_pb2json()
{
    tutorial::Person person;
    person.set_name("张三");
    person.set_info("张三 info");
    std::string json_str;
    google::protobuf::util::MessageToJsonString(person, &json_str);
    spdlog::info("json person:{}", json_str);

    std::string decoded_info = base64_decode("5byg5LiJIGluZm8=");
    // 解码后的字符串应该是 UTF-8 编码的中文字符串
    spdlog::info("Decoded info: {}", decoded_info);   // 打印解码后的中文

    tutorial::Person person2;
    google::protobuf::util::JsonStringToMessage(json_str, &person2);

    spdlog::info("person2 name:{} info: {}", person2.name(), person2.info());   // 打印解码后的中文
}


int main()
{
    //    test_reflect();
    //    test_pb_value();
    test_pb2json();
    return 0;
}
