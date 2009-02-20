#include "WebFile.h"
#include <tagaligner/tagaligner-generic.h>

WebFile::WebFile()
{
	this->initialized=false;
}

WebFile::~WebFile(){}

bool WebFile::Initialize(string path)
{
	string str_temp;
	filebuf *fb;
	istream *in;
	bool exit=true, found_ext;
	int i;
	ifstream fin;
	string text, content, aux;
	vector<string> elem_list;
	vector<int> tags;
	if(GlobalParams::GetTextCatConfigFile()=="")
		throw "TextCat's configuration file has not been specified. Please, define it in the Bitextor's configuration file.";
	else{
		try{
			//We clean the format and convert to UTF8
			FilePreprocess::PreprocessFile(path);
			//We set the file path
			this->path=path;
			//We set the extension of the file
			try{
				this->file_type=path.substr(path.find_last_of('.')+1);
			}
			catch(std::out_of_range& e){
				this->file_type="";
			}

			//We convert the extenssion of the file to lower characters to make it compatible whith the accepted extenssions in configuration file.
			/*transform(this->file_type.begin(), this->file_type.end(), this->file_type.begin(), ::tolower);
			//We open the file to process it whith the lexic analyzer to separate the tags and the text.
			found_ext=false;
			for(i=0;i<GlobalParams::GetAcceptedExtenssions().size() && found_ext==false;i++){
				if(this->file_type==GlobalParams::GetAcceptedExtenssions()[i])
					found_ext=true;
			}
			if(!found_ext){
				exit=false;
			}
			else{*/
				fin.open(path.c_str());
				if(fin.is_open()){
					fin>>content;
					while(!fin.eof()){
						fin>>aux;
						content+=" "+aux;
					}
					fin.close();
					elem_list=TagAligner_generic::SplitFilterText(content);
					text="";
					for(i=0;i<elem_list.size();i++){
						if(TagAligner_generic::isTag(elem_list[i]))
							tags.push_back(GlobalParams::GetHTMLTagValue(elem_list[i]));
						else{
							tags.push_back(elem_list[i].length());
							text+=elem_list[i];
						}
					}
					//We set the tag list
					this->tag_list=tags;
					if(GlobalParams::GetGuessLanguage()){
						//We gess the language and set it
						void *h = textcat_Init(GlobalParams::GetTextCatConfigFile().c_str());
						str_temp=textcat_Classify(h, text.c_str(), text.length());
						this->lang=str_temp.substr(1,str_temp.find_first_of("]")-1);
						if(str_temp[0]!='[')
							exit=false;
						textcat_Done(h);
						delete in;
						h=NULL;
					}
					else{
						cout<<"Set the language for the file "<<this->path<<" : ";
						cin>>this->lang;
					}
				}
				else
					exit=false;
			//}
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

string WebFile::GetLang()
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

vector<int> WebFile::GetTagArray()
{
	if(this->initialized==false)
		throw "Object not initialized.";
	else
		return tag_list;
}

bool WebFile::IsInitialized()
{
	return this->initialized;
}