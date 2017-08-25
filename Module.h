#ifndef __MODULE_H__
#define __MODULE_H__


class Module
{
	public:
		Module();
		~Module();
		int GetName(char*, int len);
		int Load();
		int Init();
	private:
		char* m_pName;
};
