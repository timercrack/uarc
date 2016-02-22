#pragma once

#include <memory>

class CFomular {

public:

	CFomular();
	~CFomular();

	void AddConstants();
	void AddVariable(const char* x, double& val);
	void AddFunction(const char* name, const char* expr, const char* v0);
	void AddFunction(const char* name, const char* expr, const char* v0, const char* v1);
	void AddFunction(const char* name, const char* expr, const char* v0, const char* v1, const char* v2);
	void AddFunction(const char* name, const char* expr, const char* v0, const char* v1, const char* v2, const char* v3);
	void AddFunction(const char* name, const char* expr, const char* v0, const char* v1, const char* v2, const char* v3, const char* v4);
	void AddFunction(const char* name, const char* expr, const char* v0, const char* v1, const char* v2, const char* v3, const char* v4, const char* v5);

	const char*
			CompileError();
	int 	Compile(const char* expr);
	double 	Value();

	void clear();

private:

	struct impl;
    std::unique_ptr<impl> _impl;
};
