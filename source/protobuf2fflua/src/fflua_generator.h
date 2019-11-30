// Ryan Wang @KOOGame
// protobuf插件, 自动生成一些fflua的注册代码
#include<google/protobuf/compiler/code_generator.h>
#include <google/protobuf/descriptor.h>
namespace google
{
	namespace protobuf
	{
		namespace io
		{
			class Printer;
		}
		class Descriptor;
	}
}

using namespace std;
using namespace google::protobuf;

class FFluaGenerator : public compiler::CodeGenerator
{
public:

	// Generates code for the given proto file, generating one or more files in
	// the given output directory.
	//
	// A parameter to be passed to the generator can be specified on the
	// command line.  This is intended to be used by Java and similar languages
	// to specify which specific class from the proto file is to be generated,
	// though it could have other uses as well.  It is empty if no parameter was
	// given.
	//
	// Returns true if successful.  Otherwise, sets *error to a description of
	// the problem (e.g. "invalid parameter") and returns false.
	virtual bool Generate(const FileDescriptor* file,
		const string& parameter,
		compiler::GeneratorContext* generator_context,
		string* error) const;
private:

	void NamespaceOpen(io::Printer& print, const FileDescriptor& file_descriptor) const;
	void NamespaceClose(io::Printer& print, const FileDescriptor& file_descriptor) const;
	void MessageFieldGen(io::Printer& printer, const Descriptor* descriptor) const;
	void AllMessageFieldReg(io::Printer& printer, const FileDescriptor& file_descriptor) const;
	void EnumTemplateSpecializtion(io::Printer& printer, const Descriptor* message_descriptor, string prefix = "") const;
	void EnumForLuaGlobal(io::Printer& printer, const FileDescriptor& file) const;
};
