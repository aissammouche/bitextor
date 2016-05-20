/*
 * Autor: Miquel Esplà i Gomis [miquel.espla@ua.es]
 * Any: 2009 
 */

#include "WebFile.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <tre/regex.h>
#include <libtagaligner/ConfigReader.h>
#include <libtagaligner/FragmentedFile.h>

WebFile::WebFile()
{
	this->initialized=false;
	this->url=NULL;
	text_size=0;
}

WebFile::~WebFile()
{
	delete url;
}

bool WebFile::IsAlphabetic(const wchar_t& car){
	int status;

	regex_t re;
	wstring pattern = L"[0-9]";
	wchar_t text[2];
	text[0] = car;
	text[1] = L'\0';

	if (regwcomp(&re, pattern.c_str(), REG_EXTENDED|REG_NOSUB) != 0)
		return false;

	status = regwexec(&re, text, (size_t) 0, NULL, 0);
    regfree(&re);

    return (status == 0);
}

void WebFile::GetNonAplha(wstring text){
	unsigned int i;
	wstring st=L"";

	for(i=0;i<text.length();i++){
		if(IsAlphabetic(text[i])){
			st+=text[i];
		}
		else{
			if(st!=L""){
				numbers_vec.push_back(atoi(Config::toString(st).c_str()));
				st=L"";
			}
		}
	}
	if(st!=L"")
		numbers_vec.push_back(atoi(Config::toString(st).c_str()));
}

bool WebFile::Initialize(const string &path, Url *url)
{
	wstring str_temp;
	bool exit=true;
	unsigned int i;
	ifstream fin;
	wstring text, content;
	vector<int> tags;
	FragmentedFile ffile;
	unsigned int pos_aux;

	if(GlobalParams::GetTextCatConfigFile()==L"")
		throw "TextCat's configuration file has not been specified. Please, define it in the Bitextor's configuration file.";
	else{
		try{
			//We clean the format and convert to UTF8
			FilePreprocess::PreprocessFile(path);
			//We set the file path
			this->path=path;
			this->url=url;
			
			
			//ObtainURL();
			//We set the extension of the file
			try{
				pos_aux=path.find_last_of('.')+1;
				if(path[pos_aux]!=L'/')
					this->file_type=path.substr(pos_aux);
				else
					this->file_type="";
			}
			catch(std::out_of_range& e){
				this->file_type="";
			}
			if(ffile.LoadFile(path)){
				ffile.Compact();
				//ffile.SplitInSentences();
				/*f=fopen((path+".xml").c_str(), "w");
				if(f){
					fputws(ffile.toXML().c_str(),f);
					fclose(f);*/
					
					for(i=0;i<ffile.getSize();i++){
						if(ffile.isTag(i))
							file.push_back(ffile.getTag(i)->getCode()*(-1));
						else
							file.push_back(ffile.getText(i)->getLength());
					}

					text=ffile.getFullText(true);
					GetNonAplha(text);
					text_size=text.size();
					//We set the tag list
					if(GlobalParams::GetGuessLanguage()){
						//We gess the language and set it
						void *h = textcat_Init(Config::toString(GlobalParams::GetTextCatConfigFile()).c_str());
						str_temp=Config::toWstring(textcat_Classify(h, Config::toString(text).c_str(), text.length()));
						this->lang=str_temp.substr(1,str_temp.find_first_of(L"]")-1);
						if(str_temp[0]!='['){
							exit=false;
							GlobalParams::WriteLog(L"Language of "+Config::toWstring(path)+L" couldn't be guessed.");
						}
						else
							GlobalParams::WriteLog(L"File "+Config::toWstring(path)+L" loaded correctly (Language: "+this->lang+L").");
						textcat_Done(h);
						h=NULL;
					}
					else{
						cout<<"Set the language for the file "<<this->path<<" : ";
						wcin>>this->lang;
					}
				/*}
				else
					exit=false;*/
			}
			else{
				exit=false;
				GlobalParams::WriteLog(L"File "+Config::toWstring(path)+L" couldn't be loaded.");
			}
		}
		catch(...){
			exit=false;
			this->initialized=false;
		}
		if(exit)
			this->initialized=true;
		else
			this->initialized=false;
	}
	return exit;
}

wstring WebFile::GetLang()
{
	if(this->initialized==false)
		throw "Object not initialized";
	else
		return this->lang;
}

string WebFile::GetPath()
{
	if(this->initialized==false)
		throw "Object not initialized";
	else
		return this->path;
}

string WebFile::GetFileType()
{
	if(this->initialized==false)
		throw "Object not initialized";
	else
		return this->file_type;
}

bool WebFile::IsInitialized()
{
	return this->initialized;
}

vector<int>* WebFile::GetNumbersVector()
{
	return &numbers_vec;
}
	
unsigned int WebFile::GetTextSize()
{
	return text_size;
}

wstring WebFile::toXML()
{
	unsigned int i;
	wostringstream *ss=new wostringstream();
	*ss<<text_size;
	wstring exit= L"<file url=\""+url->GetCompleteURL()+L"\" lang=\""+lang+L"\" >";
	exit+=L"\n\t<path>"+Config::toWstring(path)+L"</path>";
	exit+=L"\n\t<text_size value=\""+ss->str()+L"\" />";
	delete ss;
	if(file.size()>0){
		exit+=L"\n\t<fingerprint>";
		for(i=0;i<file.size();i++){
			ss=new wostringstream();
			*ss<<file.at(i);
			exit+=L"\n\t\t<fragment value=\""+ss->str()+L"\" />";
			delete ss;
		}
		exit+=L"\n\t</fingerprint>\n</file>";
	}
	return exit;
}

bool WebFile::loadXML(xmlNode* node, Url *url){
	xmlNode *cur_node, *fragment_node, *txt_node;
	xmlChar * url_att= xmlCharStrdup("url"), *lang_att=xmlCharStrdup("lang"), *value_att=xmlCharStrdup("value");
	
	if(url!=NULL)
		this->url=url;
	else
		this->url=new Url(Config::xmlToWstring(xmlGetProp(node, url_att)));

	this->lang=Config::xmlToWstring(xmlGetProp(node, lang_att));

	
	for(cur_node = node->xmlChildrenNode; cur_node; cur_node = cur_node->next) {
		if(cur_node->type==XML_ELEMENT_NODE && cur_node->name!=NULL){
			if(Config::xmlToWstring((xmlChar*)cur_node->name)==L"path"){
				for (txt_node=cur_node->xmlChildrenNode; txt_node; txt_node = txt_node->next) {
					if(txt_node->type==XML_TEXT_NODE)
						this->path=Config::toString(Config::xmlToWstring(txt_node->content));
				}
			} else if(Config::xmlToWstring((xmlChar*)cur_node->name)==L"text_size"){
				this->text_size=atoi(Config::toString(Config::xmlToWstring(xmlGetProp(cur_node, value_att))).c_str());
			} else if(Config::xmlToWstring((xmlChar*)cur_node->name)==L"fingerprint"){
				for (fragment_node=cur_node->xmlChildrenNode; fragment_node; fragment_node = fragment_node->next) {
					if(fragment_node->type==XML_ELEMENT_NODE && fragment_node->name!=NULL && Config::xmlToWstring((xmlChar*)fragment_node->name)==L"fragment")
						this->file.push_back(atoi(Config::toString(Config::xmlToWstring(xmlGetProp(fragment_node, value_att))).c_str()));
				}
			}
		}
	}
	delete url_att;
	delete lang_att;
	delete value_att;
	this->initialized=true;
	return true;
}

WebFile::WebFile(string &path, wstring &lang, vector<int> &file, Url* url){
	this->path=path;
	this->lang=lang;
	this->file=file;
	this->url=url;
	this->initialized=true;
}

vector<int>* WebFile::GetTagArray(){
	return &file;
}

/*void WebFile::ObtainURL(){
	FILE* fin;
	wstring aux=L"";
	wint_t aux_car;
	unsigned int found;

	fin=fopen(path.c_str(),"r");

	
	if(fin){
		aux_car=getwc(fin);
		while(aux_car!=WEOF){
			if(aux_car==L'\n'){
				found=aux.find(L"<!-- Mirrored from ");
				if (found<aux.length()){
					found=aux.find_first_of(L' ',20);
					url=new Url(aux.substr(19,found-19));
					aux_car=WEOF;
				}
				else
					aux_car=getwc(fin);
				aux=L"";
			}
			else{
				aux+=(wchar_t)aux_car;
				aux_car=getwc(fin);
			}
		}
		fclose(fin);
	}
}*/

Url* WebFile::GetURL(){
	if(url!=NULL)
		return url;
	else
		return new Url(L"");
}