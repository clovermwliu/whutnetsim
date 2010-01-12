#include "StdAfx.h"
#include "FileScript.h"

namespace WhuTNetSimConfigClass{

CFileScript::CFileScript(const string& strFilePath)
:CFileConfig(strFilePath)
{
}

CFileScript::~CFileScript(void)
{
}



bool
CFileScript::InitExpressionBySectionName(string expsection,
					         			 CExpressionParse& exp,
                                         string expform)
/*
����������һ�����ʽ�ڵ����ݣ���ʼ��һ�����ʽ
������expsection ���ʽ����
      expform ���ʽ��ʽ���ڼ�����Ĭ��Ϊ"formulation"
*/
{
 CGenericConfigItem<CExpressionParse> form(*this,expsection,expform);

 if (form.GetLastError()==ERROR_CONFIG_ITEM_NOT_EXIST){

	 return false;
 }

 vector<string> v;

 //��ȡ��ͨ�����������û��渳ֵ
 int count_i= (form.MyValue()).GetCommonParamName(v);

 for (int i=0;i<count_i;++i){

	 CGenericConfigItem<double> item(*this,expsection,v[i]);

	 if (item.GetLastError()!=ERROR_CONFIG_ITEM_SUCCESS){

		 cout<<"parameter "<<v[i]<<" does not exist"<<endl;

	 }

	 (form.MyValue()).SetParamValue(v[i],item);

 }

 v.clear();

 //��ȡԶ�̸�ֵ�����������û��渳ֵ
 int count_j= (form.MyValue()).GetRemoteParamName(v);

 for (int j=0;j<count_j;++j){

	 CGenericConfigItem<string> item(*this,expsection,v[j]);

	 if (item.GetLastError()!=ERROR_CONFIG_ITEM_SUCCESS){

		 cout<<"parameter "<<v[j]<<" does not exist"<<endl;

	 }

	 if (item.MyValue() == "cur_time"){

		 (form.MyValue()).SetRemoteCallAddrs(v[j],CTest::GetValue1);

	 }else{

		 (form.MyValue()).SetRemoteCallAddrs(v[j],CTest::GetValue2);
	 }
 }

 exp=form.MyValue();
 return true;

}


bool
CFileScript::InitPredicationItemBySectionName(string pred_item_section,
									          CPredicationItem& pred,
											  string keyleft,
											  string keyright,
											  string keyop)
/*

*/
{
	CGenericConfigItem<string> l(*this,pred_item_section,keyleft);

	if (l.GetLastError()==ERROR_CONFIG_ITEM_NOT_EXIST){

		return false;
	}

	CGenericConfigItem<string> r(*this,pred_item_section,keyright);

	if (r.GetLastError()==ERROR_CONFIG_ITEM_NOT_EXIST){

		return false;
	}

	CGenericConfigItem<string> op(*this,pred_item_section, keyop);

	if (op.GetLastError()==ERROR_CONFIG_ITEM_NOT_EXIST){

		return false;
	}

	 CExpressionParse exp_tmp;
	
	if (!InitExpressionBySectionName(l.MyValue(),exp_tmp)){

		return false;
	}

	pred.SetExp(true,exp_tmp);

	exp_tmp.Initial();

	if (!InitExpressionBySectionName(r.MyValue(),exp_tmp)){

		return false;
	}

	pred.SetExp(false,exp_tmp);

	pred.SetOp(op.MyValue());

	return true;

}


bool 
CFileScript::InitConditionBySectionName(string condsection,
						         		CExpCondition& cond,
								        string condkey)
/*
���������ݽű���������һ��CExpCondition�Ķ���
������[out]cond �����õ�CExpCondition����
*/
{
	CGenericConfigItem<string> stritem(*this,condsection,condkey);
	if (stritem.GetLastError()==ERROR_CONFIG_ITEM_NOT_EXIST){

		return false;
	}
    
	map<string,CPredicationItem> table;
	string str_pred_identify;
	string str=stritem.MyValue();
	string::iterator iter=str.begin();

	while(iter!=str.end()){

		if( isspace( *iter)) {           //��ǰ�ַ����ǿո��Ʊ�����з�������
			++iter;               
			continue;

		}else if(*iter == '&' && *(++iter)=='&'){

			++iter ;

		}else if(*iter == '|' && *(++iter)=='|'){

			++iter ;

		}else if(*iter =='!' ){

			++iter ;

		}else if( isalpha( *iter) || isdigit(  *iter)){

			str_pred_identify.clear();

			do {
				str_pred_identify += (*iter);
				++iter;
			} while(isalpha( *iter ) || isdigit(  *iter )); //ȡ��ĸ+���ֵĻ�ϱ��ʽ��ֱ���ȷ���ĸ�ַ����ֵķ��Ž���
			
			//��ʼ����

			CPredicationItem preditem;

			bool bresult=InitPredicationItemBySectionName(str_pred_identify,preditem);

			if (!bresult){

				//������
				return false;
			}

			table.insert(pair<string, CPredicationItem>(str_pred_identify,preditem));


		}else{

			//���Ա���
			++iter;
		}

	}

	cond.Initial(str,table);
	return true;

}



}//end
