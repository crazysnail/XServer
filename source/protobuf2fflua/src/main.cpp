// Ryan Wang @KOOGame
// protobuf插件, 自动生成一些fflua的注册代码
#include "fflua_generator.h"
#include<google/protobuf/compiler/plugin.h>

int main(int argc, char* argv[]) {
	FFluaGenerator generator;
	return google::protobuf::compiler::PluginMain(argc, argv, &generator);
}