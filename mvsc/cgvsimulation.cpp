#include "cgvsimulation.h"
#include <vector>

#define VERSION "1.0"
#define REVISION 6 

//extern bool VAR_type[5]={true};
void OpenFiles(MVSCOption mvscoption, ofstream &FOUT_B, ofstream &FOUTSNP_B, ofstream &FOUTINDEL_B, ofstream &FOUTSV_B, ofstream &FOUTSEQ_B, ofstream &FOUTCNV_B, ofstream &FOUT_P, ofstream &FOUTSNP_P, ofstream &FOUTINDEL_P, ofstream &FOUTSV_P, ofstream &FOUTSEQ_P, ofstream &FOUTCNV_P, ofstream &FOUT_C, ofstream &FOUTSNP_C, ofstream &FOUTINDEL_C, ofstream &FOUTSV_C, ofstream &FOUTSEQ_C, ofstream &FOUTCNV_C)
{
	FOUT_B.open(mvscoption.blood_ref.c_str()), FOUTSNP_B.open(mvscoption.blood_snp_info.c_str()), FOUTINDEL_B.open(mvscoption.blood_indel_info.c_str()), FOUTSV_B.open(mvscoption.blood_sv_info.c_str());//, FOUTSEQ_B.open(mvscoption.blood_inseq_info.c_str()), FOUTCNV_B.open(mvscoption.blood_cnv_info.c_str());
	if (!FOUT_B) { cerr << "Open file " << mvscoption.blood_ref << " failure!\n"; exit(1); }

	FOUT_P.open(mvscoption.para_ref.c_str()), FOUTSNP_P.open(mvscoption.para_snp_info.c_str()), FOUTINDEL_P.open(mvscoption.para_indel_info.c_str()), FOUTSV_P.open(mvscoption.para_sv_info.c_str());//, FOUTSEQ_P.open(mvscoption.para_inseq_info.c_str()), FOUTCNV_P.open(mvscoption.para_cnv_info.c_str());
	if (!FOUT_P) { cerr << "Open file " << mvscoption.para_ref << " failure\n"; exit(1); }

	FOUT_C.open(mvscoption.cancer_ref.c_str()), FOUTSNP_C.open(mvscoption.cancer_snp_info.c_str()), FOUTINDEL_C.open(mvscoption.cancer_indel_info.c_str()), FOUTSV_C.open(mvscoption.cancer_sv_info.c_str());//, FOUTSEQ_C.open(mvscoption.cancer_inseq_info.c_str()), FOUTCNV_C.open(mvscoption.cancer_cnv_info.c_str());
	if (!FOUT_C) { cerr << "Open file " << mvscoption.cancer_ref << " failure\n"; exit(1); }
}

void snp_simulation(const string &chr, const string& seq, double snp_variation_ratio, map<string, map<int, var_info> >& chr2pos2var_info, char level)
{
	map<string, map<int, var_info> >::iterator chr2pos2var_info_it = chr2pos2var_info.find(chr);
	if (chr2pos2var_info_it == chr2pos2var_info.end()) {
		map<int, var_info> pos2var_info;
		chr2pos2var_info_it = chr2pos2var_info.insert(make_pair(chr, pos2var_info)).first;
	}

	int Chr_len=seq.length();//length=133,851,895  10*8
	int sum=int(Chr_len*snp_variation_ratio);
	cout<<"Initialization SNP count="<<sum<<endl;
	int times=0, ref_position=-1;
	while (sum)
	{
	   	//int count=0;
	    ref_position=rand()%Chr_len;
		//  cerr<<"SNP count="<<++count<<endl;
		if((seq.at(ref_position)!='N')&&(ref_position-flank>=0)&&(ref_position+flank<=Chr_len))
		{
			if (isCanInsert(ref_position + 1, 1, chr2pos2var_info_it->second))
			{
				var_info ins;
			 	//snp sites
				ins.ref = seq[ref_position];//string start from 0
				string AB=SNP_var(ins.ref);
				//storage structure
				//var_info ins = {"snp", AB[0], AB[1], 1, 1, level};
				ins.var_type = "snp";
				ins.seq1 = AB[0];
				ins.length1 = 1;
				ins.seq2 = AB[1];
				ins.length2 = 1;
				ins.period = level;
				//var_info ins = {"snp", AB[0], AB[1], 1, 1, level};
				chr2pos2var_info_it->second.insert(make_pair(ref_position+1, ins));//map<int, var_info>
				++times;
			 	if(times%1000==0&&times!=0)
			 	{
					cout<<times<<" SNP simulation finished ..."<<endl;
				}
				if(times==sum)
				{
					cout<<times<<" SNP simulation finished!"<<endl;
					break;
				}

			}
		}
	}
}


//small indel seq length is less than 20, and 90 percent is less than 5, without N area 
//assume homozygous vs heterozygous 1:4 for small indel
void indel_simulation(const string &chr, const string& seq, double indel_variation_ratio, map<string, map<int, var_info> >& chr2pos2var_info, char level)
{	
	map<string, map<int, var_info> >::iterator chr2pos2var_info_it = chr2pos2var_info.find(chr);
	if (chr2pos2var_info_it == chr2pos2var_info.end()) {
		map<int, var_info> pos2var_info;
		chr2pos2var_info_it = chr2pos2var_info.insert(make_pair(chr, pos2var_info)).first;
	}

	int Chr_len=seq.length();
	int sum=int(Chr_len*indel_variation_ratio);	//if sum=10000 times
	int del_count=0;
	int ins_count=0;
	int del_Homozygous_count=0;
	int ins_Homozygous_count=0;
	int del_Heterozygous_count=0;
	int ins_Heterozygous_count=0;
	int del_Heterozygous_chainA=0;
	int ins_Heterozygous_chainA=0;
	int del_Heterozygous_chainB=0;
	int ins_Heterozygous_chainB=0;


	//variation type
	for(int i=0;i<sum;i++)
	{
		int insOrdel=rand()%2;
		if(insOrdel==0){del_count++;}
	}
	ins_count=sum-del_count;
	//#pragma omp critical
	//cout<<"Initialization insertion:\tins_count="<<ins_count<<"\tdel_count="<<del_count<<endl;
	//deletion Homozygous OR Heterozygous
	for(int i=0;i<del_count;i++)
	{
		int zygous=rand()%5;//homozygous:heterozygous=1:4
		if(zygous==0){del_Homozygous_count++;}
	}
	//insertion Homozygous OR Heterozygous
	for(int i=0;i<ins_count;i++)
	{
		int zygous=rand()%5;//homozygous:heterozygous=1:4
		if(zygous==0){ins_Homozygous_count++;}
	}
	del_Heterozygous_count=del_count-del_Homozygous_count;
    ins_Heterozygous_count=ins_count-ins_Homozygous_count;

/*#pragma omp critical
{
	cout<<"Initialization deletion:\tdel_Heterozygous_count="<<del_Heterozygous_count<<"\tdel_Homozygous_count="<<del_Homozygous_count<<endl;
	cout<<"Initialization insertion:\tins_Heterozygous_count="<<ins_Heterozygous_count<<"\tins_Homozygous_count="<<ins_Homozygous_count<<endl;
}*/
	//deletion		ChainAB

	for(int i=0;i<del_Heterozygous_count;i++)
	{
		int zygous=rand()%2;
		if(zygous==0){del_Heterozygous_chainA++;}
	}
	//insertion	ChainAB
	for(int i=0;i<ins_Heterozygous_count;i++)
	{
		int zygous=rand()%2;
		if(zygous==0){ins_Heterozygous_chainA++;}
	}
	del_Heterozygous_chainB=del_Heterozygous_count-del_Heterozygous_chainA;
	ins_Heterozygous_chainB=ins_Heterozygous_count-ins_Heterozygous_chainA;
//#pragma omp critical
//{
	cout<<"Initialization insertion:\tins_count="<<ins_count<<"\tdel_count="<<del_count<<endl;
	cout<<"Initialization deletion:\tdel_Heterozygous_count="<<del_Heterozygous_count<<"\tdel_Homozygous_count="<<del_Homozygous_count<<endl;
	cout<<"Initialization insertion:\tins_Heterozygous_count="<<ins_Heterozygous_count<<"\tins_Homozygous_count="<<ins_Homozygous_count<<endl;
	cout<<"Initialization deletion:\tdel_Heterozygous_chainB="<<del_Heterozygous_chainB<<"\tdel_Heterozygous_chainA="<<del_Heterozygous_chainA<<endl;
	cout<<"Initialization insertion:\tins_Heterozygous_chainB="<<ins_Heterozygous_chainB<<"\tins_Heterozygous_chainA="<<ins_Heterozygous_chainA<<endl;
//}

	int times=0;
	while(del_count)//deletion Variation times
	{
		int lens=get_insert_length();
		//	cout<<"lens="<<lens<<endl;
		int ref_position=-1;
		int found=1; 
		string str("");
		//int count=0;

		ref_position=rand()%Chr_len;
	    if(ref_position+lens+flank>Chr_len || ref_position-flank<0){continue;}//deletion is out of Reference

		//cerr<<"deletion count="<<++count<<endl;
		str=seq.substr(ref_position,lens);
		found=str.find('N',0);
		if(found==string::npos)
		{
			if (!isCanInsert(ref_position + 1, lens, chr2pos2var_info_it->second)) {continue;}
			//isSmallAndSmallOverlap(ref_position + 1, lens, low_bound_map_it, up_bound_map_it, pos2var_info.end())){continue;}
			//Homozygous and Heterozygous for Chain AB
			var_info ins;
			ins.period=level;
			ins.var_type = "del";
			if(times<=del_Homozygous_count)	//Homozygous
			{	
				ins.seq1=str;
				ins.length1=lens;
				ins.seq2=ins.seq1;
				ins.length2=lens;
			}//Heterozygous chainA
			else if(times<=del_Homozygous_count+del_Heterozygous_chainA)
			{				
				ins.seq1=str;
				ins.length1=lens;
				ins.seq2="H";
				ins.length2=0;
			}//Heterozygous chainB
			else if(times<=del_count)
			{
				ins.seq1="H";
				ins.length1=0;						
				ins.seq2=str;
				ins.length2=lens;
			}
			chr2pos2var_info_it->second.insert(make_pair(ref_position+1,ins));//map<int, pair<string, var_info> >
			++times;

			if(times%1000==0&&times!=0)
			{
			 	//#pragma omp critical
				cout<<times<<" small Deletion Simulation finished ..."<<endl;
			}
			if(times == del_count)
			{
			 	//#pragma omp critical
				cout << times << " small Deletion Simulation finished!" << endl;
				break;
			}
		}

	}

	times=0;
	while (ins_count)
	{
		int lens=get_insert_length();
		//int count=0;
		int ref_position=rand()%Chr_len;
		if (ref_position-flank<0 || ref_position+1+flank>Chr_len){continue;}//insertion is out of Reference
		//	cerr<<"deletion count="<<++count<<endl;
	 	if(seq.at(ref_position)!='N')
		{ 
			if (!isCanInsert(ref_position + 1, 1, chr2pos2var_info_it->second)) {continue;}
			//Select the insertion sequence
			string fragment("");//insertion sequence
			{
				const char CCH[] = "ATCG";
				char ch[21]={0};
				for (int i = 0; i < lens; ++i)
				{
					int x = rand() / (RAND_MAX / (sizeof(CCH) - 1));
					ch[i] = CCH[x];
				}
				fragment.assign(ch);
			}
			var_info ins;
			if(times<=ins_Homozygous_count)//Homozygous
			{	
				ins.seq1=fragment;
				ins.length1=lens;
				ins.seq2=fragment;
				ins.length2=lens;
			}
			//Heterozygous chainA
			else if(times<=ins_Homozygous_count+ins_Heterozygous_chainA)
			{ 
				ins.seq1=fragment;
				ins.length1=lens;
				ins.seq2="H";
				ins.length2=0;
			}
			//Heterozygous chainB
			else if(times<=ins_count)
			{									 
				ins.seq1="H";
				ins.length1=0;						
				ins.seq2=fragment;
				ins.length2=lens;
			}
			ins.period=level;
			ins.var_type="ins";
			chr2pos2var_info_it->second.insert(make_pair(ref_position+1,ins));
			++times;
			
			//test code
			cout<<times<<" Test:small Insertion Simulation ..."<<endl;
			//test code

			if(times%1000==0&&times!=0)
			{
				//#pragma omp critical
				cout<<times<<" small Insertion Simulation finished ..."<<endl;
			}
			if(times==ins_count)
			{
				cout<<times<<" small Insertion Simulation finished ..."<<endl;
				break;
			}
		}
	}

}
/*
//inversion length is from 1000bp to 1000000bp
//U:upper limit, L:lower limit
void inversion_simulation(const string &chr, const string& seq, double inv_variation_ratio, map<string, map<int, sv_var_info> >& chr2pos2sv_info, map<string, map<int, int> >& chr2nbg2nend, char level)
{
	map<string, map<int, sv_var_info> >::iterator chr2pos2sv_info_it = chr2pos2sv_info.find(chr);
	if (chr2pos2sv_info_it == chr2pos2sv_info.end()) {
		map<int, sv_var_info> pos2sv_info;
		chr2pos2sv_info_it = chr2pos2sv_info.insert(make_pair(chr, pos2sv_info)).first;
	}

	//the total number of inversion variation
    int seq_length = seq.length();
    int inv_variation_num = int(seq_length * inv_variation_ratio);
	int total=inv_variation_num;
	//	#pragma omp critical
	cout<<inv_variation_num<<" Large inversion simulation Start ..."<<endl;
    //srand(time(0));
    //inv(inverstion),del(deletion),ins(insertion)
    int inv_pos, inv_length;
    //inversion simulation
	
    while (inv_variation_num)
    {
		inv_length = rand() % kInversionLenU + kInversionLenL;
        inv_pos = rand() % (seq_length - inv_length) + 1;
		if (inv_pos - kLargeflank < 0 || inv_pos + inv_length - 1 + kLargeflank > seq_length){continue;}
        if (isCanInsert(inv_pos, inv_length, chr2pos2sv_info_it->second) && !isInNArea(chr, inv_pos, inv_length, chr2nbg2nend))
        {
            //assume homozygous vs heterozygous 1:2, ho_he == 0 -> homozygous, ho_he ==1||2->heterozygous
            int ho_he = rand() % 3;
            //allele = 0,1,2. 0:allele 1 inversion,1:allele 2 inversion, 2 :both inversion
            int allele;
            if (ho_he == 0)
                allele = 2;
            else
            {
                allele = rand() % 2;
            }
            sv_var_info inv_var_info = {"inv", allele, inv_length,  level};
            chr2pos2sv_info_it->second.insert(make_pair(inv_pos, inv_var_info));
            --inv_variation_num;
			if((total-inv_variation_num)%10==0&&(total-inv_variation_num)!=0)
			{
			//	 #pragma omp critical
				 cout<<total-inv_variation_num<<" Large inversion simulation finished ..."<<endl;
			}
		}
	}
		
}
*/




// length is from 1000bp to 1000000bp
//U:upper limit, L:lower limit
void del_copy_inv_simulation(const string &chr, const string& seq, double variation_ratio, map<string, map<int, sv_var_info> >& chr2pos2sv_info, map<string, map<int, insertion_info> > &chr2pos2insertion_info, map<string, map<int, int> >& chr2nbg2nend, string type, char level)
{
	map<string, map<int, sv_var_info> >::iterator chr2pos2sv_info_it = chr2pos2sv_info.find(chr);
	if (chr2pos2sv_info_it == chr2pos2sv_info.end()) {
		map<int, sv_var_info> pos2sv_info;
		chr2pos2sv_info_it = chr2pos2sv_info.insert(make_pair(chr, pos2sv_info)).first;
	}

	map<string, map<int, insertion_info> >::iterator chr2pos2insertion_info_it = chr2pos2insertion_info.find(chr);
	if (chr2pos2insertion_info_it == chr2pos2insertion_info.end()) {
		map<int, insertion_info> pos2insertion_info;
		chr2pos2insertion_info_it = chr2pos2insertion_info.insert(make_pair(chr, pos2insertion_info)).first;
	}

	//the total number of variation
    int seq_length = seq.length();
    int variation_num = int(seq_length * variation_ratio);
	int total = variation_num;
	//	#pragma omp critical
	cout << variation_num << " Large " << type << " simulation Start ..."<<endl;
    //srand(time(0));
    //inv(inverstion),del(deletion),copy
    int pos, length;
    //inversion simulation
	
    while (variation_num)
    {
		length = rand() % kInversionLenU + kInversionLenL;
        pos = rand() % (seq_length - length) + 1;
		if (pos - kLargeflank < 0 || pos + length - 1 + kLargeflank > seq_length){continue;}
        if (isCanInsert(pos, length, chr2pos2sv_info_it->second) && !isInNArea(chr, pos, length, chr2nbg2nend) && (chr2pos2insertion_info_it == chr2pos2insertion_info.end() || !isLocked(pos, length, chr2pos2insertion_info_it->second)))
        {
            //assume homozygous vs heterozygous 1:2, ho_he == 0 -> homozygous, ho_he ==1||2->heterozygous
            int ho_he = rand() % 3;
            //allele = 0,1,2. 0:allele 1 inversion,1:allele 2 inversion, 2 :both inversion
            int allele;
            if (ho_he == 0)
                allele = 2;
            else
            {
                allele = rand() % 2;
            }
            sv_var_info large_var_info = {type, allele, length, level};
            chr2pos2sv_info_it->second.insert(make_pair(pos, large_var_info));
            --variation_num;
			if((total-variation_num)%10==0&&(total-variation_num)!=0)
			{
			//	 #pragma omp critical
				 cout << total - variation_num << " Large " << type << " simulation finished ..."<<endl;
			}
		}
	}
		
}
///
/*
void deletion_simulation(const string &chr, const string& seq, double del_variation_ratio, map<string, string> &id2seq, vector<string> &id_vec, map<string, map<int, insertion_info> > &chr2pos2insertion_info,  map<string, map<int, sv_var_info> >& chr2pos2sv_info, map<string, map<int, int> >& chr2nbg2nend, map<pair<int, int>, pair<int, char> >& pos2allele, char level)
{
	map<string, map<int, sv_var_info> >::iterator chr2pos2sv_info_it = chr2pos2sv_info.find(chr);
	if (chr2pos2sv_info_it == chr2pos2sv_info.end()) {
		map<int, sv_var_info> pos2sv_info;
		chr2pos2sv_info_it = chr2pos2sv_info.insert(make_pair(chr, pos2sv_info)).first;
	}

	//the total number of deletion variation
    int seq_length = seq.length();
    int del_variation_num = int(seq_length * del_variation_ratio);
	int total=del_variation_num;
	pair<map<int, sv_var_info>::iterator, bool> ret;
	//	#pragma omp critical
	cout<<del_variation_num<<" Large deletion simulation Start ..."<<endl;
    //srand(time(0));
    //inv(inverstion),del(deletion),ins(insertion)
    int del_pos, del_length;
	//for translocation
	char is_translocation = '.';
    //inversion simulation
    while (del_variation_num)
    {
		del_length = rand() % kInversionLenU + kInversionLenL;
        del_pos = rand() % (seq_length - del_length) + 1;
		if (del_pos - kLargeflank < 0 || del_pos + del_length - 1 + kLargeflank > seq_length){continue;}
        if (isCanInsert(del_pos, del_length, chr2pos2sv_info_it->second) && !isInNArea(chr, del_pos, del_length, chr2nbg2nend))
        {
            //assume homozygous vs heterozygous 1:2, ho_he == 0 -> homozygous, ho_he ==1||2->heterozygous
            int ho_he = rand() % 3;
            //allele = 0,1,2. 0:allele 1 deletion,1:allele 2 deletion, 2 :both deletion
            int allele;
            if (ho_he == 0)
				allele = 2;
			else
            {
                allele = rand() % 2;
            }

            sv_var_info del_var_info = {"ldel", allele, del_length, level};
            chr2pos2sv_info_it->second.insert(make_pair(del_pos, del_var_info));

			int del_and_tran = rand() % kTranslocationDenominator;
			if (del_and_tran < kTranslocationNumerator) { 
			//	is_translocation = 't';
				insertion_simulation(id2seq, id_vec, chr2pos2insertion_info, chr2pos2sv_info, map<string, map<int, int> >& chr2nbg2nend, chr, sv_var_info_it); 
			}
            pos2allele.insert(make_pair(make_pair(del_pos, del_pos + del_length - 1), make_pair(allele, level)));
            --del_variation_num;
			if((total-del_variation_num)%10==0&&(total-del_variation_num)!=0)
			{
				// #pragma omp critical
				cout<<total-del_variation_num<<" Large deletion simulation finished ..."<<endl;
			}
		}
	}
}
*/
/*
void insertion_simulation(const string &chr, const string& seq, double ins_variation_ratio, map<string, string> &virus2seq, map<string, map<int, sv_var_info> >& chr2pos2sv_info, map<string, map<int, int> >& chr2nbg2nend, map<pair<int, int>, map<pair<int, int>, pair<int, char> > >& pos2cppos_allele, char level)
{
	map<string, map<int, sv_var_info> >::iterator chr2pos2sv_info_it = chr2pos2sv_info.find(chr);
	if (chr2pos2sv_info_it == chr2pos2sv_info.end()) {
		map<int, sv_var_info> pos2sv_info;
		chr2pos2sv_info_it = chr2pos2sv_info.insert(make_pair(chr, pos2sv_info)).first;
	}

	int seq_length = seq.length();
	int ins_variation_num = int(seq_length * ins_variation_ratio);
	int total=ins_variation_num;

	//	#pragma omp critical
	cout<<ins_variation_num<<" Large insertion simulation Start ..."<<endl;
    //srand(time(0));
    //Exogenous insertion,Endogenous insertion
    //for distinguishing Exogenous insertion,Endogenous insertion
    //0:out, 1-3:in
    int cp_length, cp_pos, ins_pos;
    string subseq;
    while (ins_variation_num)
    {
		int in_or_out = rand() % 4;
		int virus_num = virus2seq.size();
		if (in_or_out == 0 && virus_num > 0)
        {
			int selected_virus_index = 0; 
			if (virus_num > 1) {
				selected_virus_index = rand() % virus_num;
			}
			map<string, string>::iterator map_it = virus2seq.begin();
			for (int i = 0; i < selected_virus_index; i++)
				++map_it;
		
			string virus_id = map_it->first;
			string virus_seq = map_it->second;
			
			int virus_seq_length = virus_seq.length();

        	cp_length = rand() % (kIndelLenU < virus_seq_length ? kIndelLenU : virus_seq_length - kIndelLenL) + kIndelLenL;
            cp_pos = rand() % (virus_seq_length - cp_length) + 1;
            subseq = virus_seq.substr(cp_pos - 1, cp_length);
            while (1)
			{
			    ins_pos = rand() % seq_length + 1;
			    if (ins_pos - kLargeflank < 0 || ins_pos + kLargeflank > seq_length){continue;}
                if (!isInNAreadForIns(chr, ins_pos, chr2nbg2nend) && isCanInsert(ins_pos, 1, chr2pos2sv_info_it->second))
                {
                	int ho_he = rand() % 3;
                    //allele = 0,1,2. 0:allele 1 deletion,1:allele 2 deletion, 2 :both deletion
                    int allele;
                    if (ho_he == 0)
						allele = 2;
                    else
                    {
                        allele = rand() % 2;
                    }
                    sv_var_info ins_var_info = {"lins", allele, subseq, subseq, cp_length, 'o', level};
                    chr2pos2sv_info_it->second.insert(make_pair(ins_pos, ins_var_info));
                    break;
                }
			}
		}
		else
		{
        	ins_cp_simulation(chr, seq, chr2pos2sv_info_it->second, chr2nbg2nend, pos2cppos_allele, level);
		}
		--ins_variation_num;
		if((total-ins_variation_num)%10==0&&(total-ins_variation_num)!=0)
		{
			  cout<<total-ins_variation_num<<" Large insertion simulation finished ..."<<endl;
		}
	}
}
*/

// string in id_vec must be a (sub) set of id2seq's keys
void insertion_simulation(map<string, string> &id2seq, vector<string> &id_vec, map<string, map<int, insertion_info> > &chr2pos2insertion_info, map<string, map<int, sv_var_info> >& chr2pos2sv_info, map<string, map<int, int> >& chr2nbg2nend, string chr_of_insert_seq, map<int, sv_var_info>::iterator &pos2sv_var_info_it) {
	int size = id_vec.size();
	int idx = rand() % size;
	string chr = id_vec[idx];
	string seq = id2seq[chr];
	int seq_length = seq.length();

	int ins_pos = rand() % seq_length + 1;
	map<string, map<int, insertion_info> >::iterator chr2pos2insertion_info_it = chr2pos2insertion_info.find(chr);
	while (ins_pos - kLargeflank < 0 || ins_pos + kLargeflank > seq_length || isInNAreadForIns(chr, ins_pos, chr2nbg2nend) || !isCanInsert(chr, ins_pos, 1, chr2pos2sv_info) || (chr2pos2insertion_info_it != chr2pos2insertion_info.end() && chr2pos2insertion_info_it->second.count(ins_pos) > 0)) { 
		ins_pos = rand() % seq_length + 1; 
	}

	insertion_info ins_info;
	ins_info.chr = chr_of_insert_seq;
	ins_info.pos2sv_var_info_it = pos2sv_var_info_it;

	if (chr2pos2insertion_info_it == chr2pos2insertion_info.end()) {
		map<int, insertion_info> pos2insertion_info;
		pos2insertion_info.insert(make_pair(ins_pos, ins_info));
		chr2pos2insertion_info.insert(make_pair(chr, pos2insertion_info));
	}
	else {
		chr2pos2insertion_info_it->second.insert(make_pair(ins_pos, ins_info));
	}
	
}

// string in id_vec must be a (sub) set of id2seq's keys
void VirusIntegrationSimulation(map<string, string> &chr2seq, map<string, string> &virus2seq, vector<string> &vec_virus, map<string, map<int, insertion_info> > &chr2pos2insertion_info, map<string, map<int, sv_var_info> >& chr2pos2sv_info, map<string, map<int, sv_var_info> >& virus2pos2sv_info, map<string, map<int, int> >& chr2nbg2nend, double variation_ratio, char level)
{
	int virus_num = vec_virus.size();
	if (virus_num == 0) {
		cerr << "Without virus sequence, cannot simulate virus integration" << endl;
		return;
	}
	int seq_length, integration_num, ins_pos, idx, cp_length, cp_pos;
	string chr, virus, virus_seq;
	map<string, map<int, sv_var_info> >::iterator virus2pos2sv_info_it;
	map<int, sv_var_info>::iterator pos2sv_info_it;
	map<string, map<int, insertion_info> >::iterator chr2pos2insertion_info_it;

	map<string, string>::iterator chr2seq_it = chr2seq.begin();
	while (chr2seq_it != chr2seq.end()) {
		chr = chr2seq_it->first;
		seq_length = chr2seq_it->second.length();
		integration_num = int(seq_length * variation_ratio);
		chr2pos2insertion_info_it = chr2pos2insertion_info.find(chr);
		while (integration_num > 0) {
			ins_pos = rand() % seq_length + 1;
			while (ins_pos - kLargeflank < 0 || ins_pos + kLargeflank > seq_length || isInNAreadForIns(chr, ins_pos, chr2nbg2nend) || !isCanInsert(chr, ins_pos, 1, chr2pos2sv_info) || (chr2pos2insertion_info_it != chr2pos2insertion_info.end() && chr2pos2insertion_info_it->second.count(ins_pos) > 0)) { 
				ins_pos = rand() % seq_length + 1;
			}

			int selected_virus_index = 0; 
			if (virus_num > 1) {
				selected_virus_index = rand() % virus_num;
			}
			virus = vec_virus[selected_virus_index];

			virus_seq = virus2seq[virus];
			
			int virus_seq_length = virus_seq.length();

        	cp_length = rand() % (kIndelLenU < virus_seq_length ? kIndelLenU : virus_seq_length - kIndelLenL) + kIndelLenL;
            cp_pos = rand() % (virus_seq_length - cp_length) + 1;
		
            //assume homozygous vs heterozygous 1:2, ho_he == 0 -> homozygous, ho_he ==1||2->heterozygous
            int ho_he = rand() % 3;
            //allele = 0,1,2. 0:allele 1 variation,1:allele 2 variation, 2 :both variation
            int allele;
            if (ho_he == 0)
                allele = 2;
            else
            {
                allele = rand() % 2;
            }

			sv_var_info virus_cp_info = {"virus_integration", allele, cp_length, level};
			virus2pos2sv_info_it = virus2pos2sv_info.find(virus);
			if (virus2pos2sv_info_it == virus2pos2sv_info.end()) {
				map<int, sv_var_info> pos2sv_info;
				pos2sv_info_it = pos2sv_info.insert(make_pair(cp_pos, virus_cp_info)).first;
				virus2pos2sv_info.insert(make_pair(virus, pos2sv_info));
			}
			else {
				pos2sv_info_it = virus2pos2sv_info_it->second.insert(make_pair(cp_pos, virus_cp_info)).first;
			}
			insertion_info ins_info;
			ins_info.chr = virus;
			ins_info.pos2sv_var_info_it = pos2sv_info_it;

			if (chr2pos2insertion_info_it != chr2pos2insertion_info.end()) {
				chr2pos2insertion_info_it->second.insert(make_pair(cp_pos, ins_info));
			}
			else {
				map<int, insertion_info> pos2insertion_info;
				pos2insertion_info.insert(make_pair(cp_pos, ins_info));
				chr2pos2insertion_info.insert(make_pair(chr, pos2insertion_info));
			}

			--integration_num;
		}

		++chr2seq_it;
	}
}
	
/*
void variation_integration(const string &seq, map<int, var_info> &pos2var_info, map<int, sv_var_info> &pos2sv_info, string &outseq1, string &outseq2)
{
	//test code
	cout << "before integration outseq size: " << seq.size() << '\t' << outseq1.size() << '\t' << outseq2.size() << endl;
	//test code
    map<int, var_info>::iterator snp_indel_map_it = pos2var_info.begin();
	
    map<int, sv_var_info>::iterator sv_map_it = pos2sv_info.begin();
    int pos, last_pos = 0, len;
	//test code
	total_len1 = 0;
	total_len2 = 0;
	//test code

    while (sv_map_it != pos2sv_info.end())
    {
		if (sv_map_it->second.var_type == "copy")
		{
			sv_map_it++;
			continue;
		}
		pos = sv_map_it->first;
		//test code 
		//	cerr << sv_map_it->second.var_type << '\t' << sv_map_it->first << '\t' << sv_map_it->second.length << endl;
		//test code

        //combine the outseq before sv
        sv_external_integration(seq, pos2var_info, snp_indel_map_it, last_pos, pos, outseq1, outseq2);
			
		//test code
		//cerr << outseq1.size() << '\t' << outseq2.size() << '\t' << "real length before large var" << endl;
		//test code

        //combine the outseq inner sv
        sv_internal_integration(seq, pos2var_info, snp_indel_map_it, sv_map_it, last_pos, pos, outseq1, outseq2);
		//test code
		cerr << outseq1.size() << '\t' << outseq2.size() << '\t' << "real length add large var" << endl;
		//test code

        sv_map_it++;
		
	}
  	pos = seq.length() + 1;
	sv_external_integration(seq, pos2var_info, snp_indel_map_it, last_pos, pos, outseq1, outseq2);
	//test code
	cerr << outseq1.size() << '\t' << outseq2.size() << '\t' << "real length" << endl;
	cout << "after integration outseq size: " << outseq1.size() << '\t' << outseq2.size() << endl;
	//test code

}
*/

void ModifySmallVariation(map<string, map<int, var_info> >& chr2pos2var_info, map<string, map<int, insertion_info> > &chr2pos2insertion_info, map<string, map<int, sv_var_info> >& chr2pos2sv_info, map<string, map<int, sv_var_info> >& virus2pos2sv_info, map<string, map<int, int> >& chr2nbg2nend, char level)
{
	map<string, map<int, insertion_info> >::iterator chr2pos2insertion_info_it = chr2pos2insertion_info.begin();
	map<string, map<int, var_info> >::iterator chr2pos2var_info_it;
	map<int, var_info>::iterator pos2var_info_it;
	map<int, insertion_info>::iterator pos2insertion_info_it;
	//delete small insertions of which insertion position is same with large insertion
	while (chr2pos2insertion_info_it != chr2pos2insertion_info.end()) {
		chr2pos2var_info_it = chr2pos2var_info.find(chr2pos2insertion_info_it->first);
		pos2insertion_info_it = chr2pos2insertion_info_it->second.begin();
		while (pos2insertion_info_it != chr2pos2insertion_info_it->second.end()) {
			int pos = pos2insertion_info_it->first;
			pos2var_info_it = chr2pos2var_info_it->second.upper_bound(pos);	
			if (pos2var_info_it != chr2pos2var_info_it->second.begin()) {
				pos2var_info_it--;
				if (pos2var_info_it->second.var_type == "ins") {
					chr2pos2var_info_it->second.erase(pos2var_info_it);
				}
				else if (pos2var_info_it->second.var_type == "del") {
					if (pos2var_info_it->first < pos && (pos2var_info_it->second.length1 + pos2var_info_it->first - 1 >= pos || pos2var_info_it->second.length2 + pos2var_info_it->first - 1 >= pos)) {
						chr2pos2var_info_it->second.erase(pos2var_info_it);
					}
				}
			}
			++pos2insertion_info_it;
		}
		++chr2pos2insertion_info_it;
	}

	map<string, map<int, sv_var_info> >::iterator chr2pos2sv_info_it = chr2pos2sv_info.begin();
	map<int, sv_var_info>::iterator pos2sv_info_it;
	while (chr2pos2sv_info_it != chr2pos2sv_info.end()) {
		chr2pos2var_info_it = chr2pos2var_info.find(chr2pos2sv_info_it->first);
		pos2sv_info_it = chr2pos2sv_info_it->second.begin();
		while (pos2sv_info_it != chr2pos2sv_info_it->second.end()) {
			int pos = pos2sv_info_it->first;
			DeleteSmallDelSpanSpecificPos(chr2pos2var_info_it->second, pos);
			int end_pos = pos + pos2sv_info_it->second.length - 1;
			DeleteSmallDelSpanSpecificPos(chr2pos2var_info_it->second, end_pos);
			++ pos2sv_info_it;
		}
		++chr2pos2sv_info_it;
	}
}

void DeleteSmallDelSpanSpecificPos(map<int, var_info> &pos2var_info, int pos)
{
	map<int, var_info>::iterator pos2var_info_it = pos2var_info.upper_bound(pos);
	if (pos2var_info_it != pos2var_info.begin()) {
		pos2var_info_it--;
		if (pos2var_info_it->second.var_type == "del") {
			if (pos2var_info_it->first < pos && (pos2var_info_it->second.length1 + pos2var_info_it->first - 1 + flank >= pos || pos2var_info_it->second.length2 + pos2var_info_it->first - 1 + flank >= pos)) {
				pos2var_info.erase(pos2var_info_it);
			}
		}
	}

}

void VariationIntegration(ofstream &FASTA, ofstream &FOUTSNP, ofstream &FOUTINDEL, ofstream &FOUTSV, map<string, string> &chr2seq, map<string, string> &virus2seq, vector<string> &vec_chr, vector<string> &vec_virus, map<string, map<int, var_info> > &chr2pos2var_info, map<string, map<int, insertion_info> > &chr2pos2insertion_info, map<string, map<int, sv_var_info> >& chr2pos2sv_info, map<string, map<int, sv_var_info> >& virus2pos2sv_info, map<string, map<int, int> >& chr2nbg2nend, char level)
{
	map<string, map<int, sv_var_info> >::iterator chr2pos2sv_info_it;
	map<string, map<int, insertion_info> >::iterator chr2pos2insertion_info_it;
	map<string, map<int, var_info> >::iterator chr2pos2var_info_it;
	for (int i = 0; i < vec_chr.size(); i ++) {
		string chr = vec_chr[i];
		string seq = chr2seq[chr];
		string outseq1, outseq2;
		chr2pos2sv_info_it = chr2pos2sv_info.find(chr);
		chr2pos2insertion_info_it = chr2pos2insertion_info.find(chr);
		chr2pos2var_info_it = chr2pos2var_info.find(chr);
		map<int, sv_var_info>::iterator pos2sv_info_it = chr2pos2sv_info_it->second.begin(); 
		map<int, insertion_info>::iterator pos2insertion_info_it = chr2pos2insertion_info_it->second.begin();
		map<int, var_info>::iterator pos2var_info_it = chr2pos2var_info_it->second.begin();


		int last_pos = 0;

		while (pos2sv_info_it != chr2pos2sv_info_it->second.end() && pos2insertion_info_it != chr2pos2insertion_info_it->second.end()) {
			cerr << "Integration 0: " << chr2pos2sv_info_it->first << endl;

			if (pos2sv_info_it->first < pos2insertion_info_it->first) {
				cerr << "Integration 1: " << chr2pos2sv_info_it->first << endl;
    			sv_external_integration(seq, chr2pos2var_info_it->second, pos2var_info_it, last_pos, pos2sv_info_it->first, outseq1, outseq2);
				cerr << "Integration 2: " << chr2pos2sv_info_it->first << endl;
				sv_internal_integration(seq, chr2pos2var_info_it->second, pos2var_info_it, pos2sv_info_it, last_pos, outseq1, outseq2);
				cerr << "Integration 3: " << chr2pos2sv_info_it->first << endl;

				++pos2sv_info_it;
			}
			else if (pos2insertion_info_it->first < pos2sv_info_it->first) {
				cerr << "Integration 4: " << chr2pos2sv_info_it->first << endl;
    			sv_external_integration(seq, chr2pos2var_info_it->second, pos2var_info_it, last_pos, pos2insertion_info_it->first, outseq1, outseq2);
				cerr << "Integration 5: " << chr2pos2sv_info_it->first << endl;
				InsertionIntegration(chr2seq, virus2seq, chr2pos2var_info, virus2pos2sv_info, pos2insertion_info_it, outseq1, outseq2);
				cerr << "Integration 6: " << chr2pos2sv_info_it->first << endl;
				++pos2insertion_info_it;
			}
			else {
				cerr << "Error: Insertion position is same with sv(deletion/inversion/copy) position" << endl;
				exit(1);
			}
		}

		while (pos2sv_info_it != chr2pos2sv_info_it->second.end()) {
        	sv_external_integration(seq, chr2pos2var_info_it->second, pos2var_info_it, last_pos, pos2sv_info_it->first, outseq1, outseq2);
			sv_internal_integration(seq, chr2pos2var_info_it->second, pos2var_info_it, pos2sv_info_it, last_pos, outseq1, outseq2);
			++pos2sv_info_it;
		}
		while (pos2insertion_info_it != chr2pos2insertion_info_it->second.end()) {
        	sv_external_integration(seq, chr2pos2var_info_it->second, pos2var_info_it, last_pos, pos2insertion_info_it->first, outseq1, outseq2);
			InsertionIntegration(chr2seq, virus2seq, chr2pos2var_info, virus2pos2sv_info, pos2insertion_info_it, outseq1, outseq2);
			++pos2insertion_info_it;
		}
		int pos = seq.length() + 1;
		cerr << "Integration 1" << endl;
        sv_external_integration(seq, chr2pos2var_info_it->second, pos2var_info_it, last_pos, pos, outseq1, outseq2);
		cerr << "Integration 2" << endl;
		string chr1=chr, chr2=chr;
		chr1.append("/1");
		chr2.append("/2");
		OutputFasta(FASTA, chr1, outseq1);
		OutputFasta(FASTA, chr2, outseq2);
		Output_small_info(chr2pos2var_info_it->second, FOUTSNP, FOUTINDEL, chr);
		OutputDelInvInfo(FOUTSV, chr2pos2sv_info_it->second, chr);
		OutputInsTranInfo(FOUTSV, chr2pos2insertion_info_it->second, chr);
	}
}

void sv_external_integration(const string& seq, map<int, var_info>& pos2var_info, map<int, var_info>::iterator& snp_indel_map_it, int& last_pos, int pos, string& outseq1, string& outseq2)
{
    int len = 0;
	//test code
	int tlen1 = 0, tlen2 = 0;
	//cerr << "newlen1" << '\t' << "last_pos1" << '\t' << "newlen2" << '\t' << "last_pos2" << '\t' << "total_len1" << '\t' << "seq_len1" << '\t' << "total_len2" << '\t' << "seq_len2" << endl;
		//test code
    while (snp_indel_map_it != pos2var_info.end())
    {
        //len = snp_indel_map_it->second.length1 > snp_indel_map_it->second.length2 ? snp_indel_map_it->second.length1 : snp_indel_map_it->second.length2;
		//test code
        if (snp_indel_map_it->first < pos)
        {
			if (snp_indel_map_it->second.var_type == "ins")
			{
				outseq1.append(seq, last_pos, snp_indel_map_it->first - 1 - last_pos);
                outseq2.append(seq, last_pos, snp_indel_map_it->first - 1 - last_pos);
				//test code
				tlen1 += snp_indel_map_it->first - 1 - last_pos;
				tlen2 += snp_indel_map_it->first - 1 - last_pos;
//cerr << snp_indel_map_it->first - 1 - last_pos << '\t' << last_pos << '\t' << snp_indel_map_it->first - 1 - last_pos << '\t' << last_pos << '\t' << tlen1 << '\t' << outseq1.size() << '\t' << tlen2 << '\t' << outseq2.size() << '\t' << "ins" << endl;
				//test code//
                if (snp_indel_map_it->second.seq1 != "H")
					outseq1.append(snp_indel_map_it->second.seq1);
                if (snp_indel_map_it->second.seq2 != "H")
                    outseq2.append(snp_indel_map_it->second.seq2);
				//test code
				tlen1 += snp_indel_map_it->second.length1;
				tlen2 += snp_indel_map_it->second.length2;
				//	addLength1 += snp_indel_map_it->second.length1;
				//	addLength2 += snp_indel_map_it->second.length2;
				//test code//
                last_pos = snp_indel_map_it->first - 1;
                snp_indel_map_it++;
			}
            else if (snp_indel_map_it->second.var_type == "del")
			{
                len = snp_indel_map_it->second.length1 > snp_indel_map_it->second.length2 ? snp_indel_map_it->second.length1 : snp_indel_map_it->second.length2;
                if (snp_indel_map_it->first + len + flank< pos)
				{
                	if (snp_indel_map_it->second.seq1 == "H" && snp_indel_map_it->second.seq2 != "H")
                    {
                    	outseq1.append(seq, last_pos, snp_indel_map_it->first + len - 1 - last_pos);
                        outseq2.append(seq, last_pos, snp_indel_map_it->first - 1 - last_pos);
						//test code
						tlen1 += snp_indel_map_it->first + len - 1 - last_pos;
						tlen2 += snp_indel_map_it->first - 1 - last_pos;
//cerr << snp_indel_map_it->first + len - 1 - last_pos << '\t' << last_pos << '\t' << snp_indel_map_it->first - 1 - last_pos << '\t' << last_pos << '\t' << tlen1 << '\t' << outseq1.size() << '\t' << tlen2 << '\t' << outseq2.size() << '\t' << "del1" << endl;
						//test code//
					}
                    else if (snp_indel_map_it->second.seq1 != "H" && snp_indel_map_it->second.seq2 == "H")
					{
                    	outseq1.append(seq, last_pos, snp_indel_map_it->first - 1 - last_pos);
                        outseq2.append(seq, last_pos, snp_indel_map_it->first + len - 1 - last_pos);
						//test code
						tlen1 += snp_indel_map_it->first - 1 - last_pos;
						tlen2 += snp_indel_map_it->first + len - 1 - last_pos;
//cerr << snp_indel_map_it->first - 1 - last_pos << '\t' << last_pos << '\t'<< snp_indel_map_it->first + len - 1 - last_pos << '\t'<< last_pos << '\t' << tlen1 << '\t' << outseq1.size() << '\t' << tlen2 << '\t' << outseq2.size() << '\t' << "del2" << endl;
						//test code//
					}
                    else if (snp_indel_map_it->second.seq1 != "H" && snp_indel_map_it->second.seq2 != "H")
					{
                    	outseq1.append(seq, last_pos, snp_indel_map_it->first - 1 - last_pos);
                        outseq2.append(seq, last_pos, snp_indel_map_it->first - 1 - last_pos);
						//test code
						tlen1 += snp_indel_map_it->first - 1 - last_pos;
						tlen2 += snp_indel_map_it->first - 1 - last_pos;
//cerr << snp_indel_map_it->first - 1 - last_pos << '\t' << last_pos << '\t' << snp_indel_map_it->first - 1 - last_pos << '\t' << last_pos << '\t'<< tlen1 << '\t' << outseq1.size() << '\t' << tlen2 << '\t' << outseq2.size() << '\t' << "del3" << endl;
						//test code//
					}
					else 
					{
						cerr << "Error: in small deletion allele" << endl;
					}
                    last_pos = snp_indel_map_it->first + len - 1;
					//test code
					//		addLength1 -= snp_indel_map_it->second.length1;
					//		addLength2 -= snp_indel_map_it->second.length2;
					//test code
                    snp_indel_map_it++;
				}
                else
				{
                	pos2var_info.erase(snp_indel_map_it++);
				}
			}
            else if (snp_indel_map_it->second.var_type == "snp")
			{
            	outseq1.append(seq, last_pos, snp_indel_map_it->first - 1 - last_pos);
                outseq2.append(seq, last_pos, snp_indel_map_it->first - 1 - last_pos);
                outseq1.append(snp_indel_map_it->second.seq1);
                outseq2.append(snp_indel_map_it->second.seq2);

				//test code
				tlen1 += snp_indel_map_it->first - last_pos;
				tlen2 += snp_indel_map_it->first - last_pos;
//cerr << snp_indel_map_it->first - last_pos << '\t' << last_pos << '\t' << snp_indel_map_it->first - last_pos << '\t' << last_pos << '\t' << tlen1 << '\t' << outseq1.size() <<  '\t' << tlen2 << '\t' << outseq2.size() << '\t' << "snp" << endl;
				//test code//

                last_pos = snp_indel_map_it->first;
                ++snp_indel_map_it;
			}
            else 
			{
				cerr << "Error1: in small deletion allele" << endl;
			}
		}
        else
		{
        	outseq1.append(seq, last_pos, pos - 1 - last_pos);
            outseq2.append(seq, last_pos, pos - 1 - last_pos);
			//test code
			tlen1 += pos - 1 - last_pos;
			tlen2 += pos - 1 - last_pos;
//cerr << pos - 1 << '\t' << last_pos << '\t' << pos - 1  << '\t' << last_pos << '\t' << tlen1 << '\t' << outseq1.size() <<  '\t' << tlen2 << '\t' << outseq2.size() << '\t' << "diff" << endl;
			//test code//
			//test code//
            last_pos = pos - 1;
            break;
        }
	}

    if (snp_indel_map_it == pos2var_info.end())
    {
    	outseq1.append(seq, last_pos, pos - 1 - last_pos);
        outseq2.append(seq, last_pos, pos - 1 - last_pos);
		//test code
		tlen1 += pos - 1 - last_pos;
		tlen2 += pos - 1 - last_pos;
//cerr << pos - 1 << '\t' << last_pos << '\t' << pos - 1 << '\t' << last_pos << '\t' << tlen1 << '\t' << outseq1.size() <<  '\t' << tlen2 << '\t' << outseq2.size() << '\t' << "no small var" << endl;
		//test code//
        last_pos = pos - 1;
	}
	//*test code
	total_len1 += tlen1;
	total_len2 += tlen2;
	cerr << tlen1 << '\t' << total_len1 << '\t' << tlen2 << '\t' << total_len2 << endl;
	//test code */

}

void InsertionIntegration(map<string, string>& chr2seq, map<string, string> &virus2seq, map<string, map<int, var_info> >& chr2pos2var_info, map<string, map<int, sv_var_info> >& virus2pos2sv_var_info, map<int, insertion_info>::iterator &pos2insertion_info_it, string& outseq1, string& outseq2)
{
	string chr = pos2insertion_info_it->second.chr;
	string seq = chr2seq[chr];

	map<int, sv_var_info>::iterator pos2sv_var_info_it = pos2insertion_info_it->second.pos2sv_var_info_it; 
	string insert_seq1, insert_seq2;
	int begin_pos = pos2sv_var_info_it->first - 1;
	int pos = begin_pos + pos2sv_var_info_it->second.length + 1;
	
	map<string, map<int, var_info> >::iterator chr2pos2var_info_it = chr2pos2var_info.find(chr);
	if (chr2pos2var_info_it != chr2pos2var_info.end()) {
		map<int, var_info>::iterator pos2var_info_it = chr2pos2var_info_it->second.upper_bound(pos2sv_var_info_it->first);
		cerr << "111" << endl;
		sv_external_integration(seq, pos2var_info, pos2var_info_it, begin_pos, pos, insert_seq1, insert_seq2);
		cerr << "112" << endl;
	
		if (pos2sv_var_info_it->second.allele == 2 || pos2sv_var_info_it->second.allele == 0) {
			cerr << "113" << endl;
			outseq1.append(insert_seq1);
			cerr << "114" << endl;
		}
		else if (pos2sv_var_info_it->second.allele == 2 || pos2sv_var_info_it->second.allele == 1) {
			cerr << "115" << endl;
			outseq2.append(insert_seq2);
			cerr << "116" << endl;
		}
		else {
			cerr << "Error allele type" << endl;
			exit(1);
		}

	}
	else {
		map<string, map<int, sv_var_info> >::iterator chr2pos2sv_var_info_it = virus2pos2sv_var_info.find(chr);
		if (chr2pos2sv_var_info_it != virus2pos2sv_var_info.end()) {
			seq = virus2seq[chr];
			map<int, sv_var_info>::iterator pos2sv_var_info_it = pos2insertion_info_it->second.pos2sv_var_info_it;	
			insert_seq1.append(seq, pos2sv_var_info_it->first - 1, pos2sv_var_info_it->second.length);
			outseq1.append(insert_seq1);
			outseq1.append(insert_seq2);
		}
		else {
			cerr << "Error chr in function InsertionIntegration():" << chr << endl;
			exit(1);
		}
	}
}

void sv_internal_integration(const string& seq, map<int, var_info>& pos2var_info, map<int, var_info>::iterator& snp_indel_map_it, map<int, sv_var_info>::iterator& sv_map_it, int& last_pos, string& outseq1, string& outseq2)
{
	//if (snp_indel_map_it->first < pos)
    if (sv_map_it->second.var_type == "ldel")
    {
    	if (sv_map_it->second.allele == 2) {
        	while (snp_indel_map_it != pos2var_info.end())
            {
           		if (snp_indel_map_it->first < sv_map_it->first + sv_map_it->second.length + flank)
				{
                	pos2var_info.erase(snp_indel_map_it++);
				}
                else if (snp_indel_map_it->first >= sv_map_it->first + sv_map_it->second.length + flank)
                    break;
                else
				{
					cerr << "Error2: in small deletion" << endl;
				}
            }
            last_pos = sv_map_it->first + sv_map_it->second.length - 1;
		}
        else if (sv_map_it->second.allele == 1) {
            ldel_internal_integration(seq, pos2var_info, snp_indel_map_it, last_pos, sv_map_it->first + sv_map_it->second.length, outseq1, 1);
        }
        else if (sv_map_it->second.allele == 0) {
        	ldel_internal_integration(seq, pos2var_info, snp_indel_map_it, last_pos, sv_map_it->first + sv_map_it->second.length, outseq2, 0);
		}
	}
	else if (sv_map_it->second.var_type == "del,translocation") {
    	if (sv_map_it->second.allele == 2) {
            last_pos = sv_map_it->first + sv_map_it->second.length - 1;
			snp_indel_map_it = pos2var_info.lower_bound(last_pos);
		}
		else {
    		string del_seq1, del_seq2;
        	sv_external_integration(seq, pos2var_info, snp_indel_map_it, last_pos, sv_map_it->first + sv_map_it->second.length, del_seq1, del_seq2);
			if (sv_map_it->second.allele == 1) {
        		outseq2.append(del_seq1);
			}
			else {
        		outseq1.append(del_seq2);
			}
		}
	}
    else if (sv_map_it->second.var_type == "inv")
	{
    	string inv_seq1, inv_seq2;
        sv_external_integration(seq, pos2var_info, snp_indel_map_it, last_pos, sv_map_it->first + sv_map_it->second.length, inv_seq1, inv_seq2);
        if (sv_map_it->second.allele == 2)
        {
        	inverse_seq(inv_seq1);
            inverse_seq(inv_seq2);
        }
        else if (sv_map_it->second.allele == 1)
		{
            inverse_seq(inv_seq2);
		}
        else if (sv_map_it->second.allele == 0)
		{
        	inverse_seq(inv_seq1);
		}
        outseq1.append(inv_seq1);
        outseq2.append(inv_seq2);
	}
}
//chain 1 deletion
void ldel_internal_integration(const string& seq, map<int, var_info>& pos2var_info, map<int, var_info>::iterator& snp_indel_map_it, int& last_pos, int pos, string& outseq, int chain)
{
	int len = 0;
	//*test code
	int tlen1 = 0, tlen2 = 0;
	//test code
    while (snp_indel_map_it != pos2var_info.end())
    {
    	//len = snp_indel_map_it->second.length1 > snp_indel_map_it->second.length2 ? snp_indel_map_it->second.length1 : snp_indel_map_it->second.length2;
        if (snp_indel_map_it->first < pos)
		{
        	if (snp_indel_map_it->second.var_type == "ins")
			{
            	switch (chain)
				{
                case 0:
                	if (snp_indel_map_it->second.seq2 != "H")
                    {
                    	outseq.append(seq, last_pos, snp_indel_map_it->first - 1 - last_pos);
                        outseq.append(snp_indel_map_it->second.seq2);
						//test code
						tlen2 += snp_indel_map_it->first - 1 - last_pos;
						tlen2 += snp_indel_map_it->second.length2;
						//test code//
                        snp_indel_map_it->second.seq1 = snp_indel_map_it->second.seq2;
                        snp_indel_map_it->second.length1 = snp_indel_map_it->second.length2;
                        last_pos = snp_indel_map_it->first - 1;
                        snp_indel_map_it++;
					}
                    else
                        pos2var_info.erase(snp_indel_map_it++);
					break;

				case 1:
                	if (snp_indel_map_it->second.seq1 != "H")
					{
                    	outseq.append(seq.substr(last_pos, snp_indel_map_it->first - 1 - last_pos));
                        outseq.append(snp_indel_map_it->second.seq1);
						//test code
						tlen1 += snp_indel_map_it->first - 1 - last_pos;
						tlen1 += snp_indel_map_it->second.length1;
						//test code//
                        snp_indel_map_it->second.seq2 = snp_indel_map_it->second.seq1;
                        snp_indel_map_it->second.length2 = snp_indel_map_it->second.length1;
                        last_pos = snp_indel_map_it->first - 1;
                        snp_indel_map_it++;
					}
                    else
                        pos2var_info.erase(snp_indel_map_it++);
				}
			}
            else if (snp_indel_map_it->second.var_type == "del")
			{
            	len = snp_indel_map_it->second.length1 > snp_indel_map_it->second.length2 ? snp_indel_map_it->second.length1 : snp_indel_map_it->second.length2;
				switch (chain)
				{
                case 0:
                    if (snp_indel_map_it->first + len < pos)
					{
                    	if (snp_indel_map_it->second.seq2 != "H")
						{
                        	outseq.append(seq, last_pos, snp_indel_map_it->first - 1 - last_pos);
							//test code
							tlen2 += snp_indel_map_it->first - 1 - last_pos;
							//test code//
                            snp_indel_map_it->second.seq1 = snp_indel_map_it->second.seq2;
                            snp_indel_map_it->second.length1 = snp_indel_map_it->second.length2;
                            last_pos = snp_indel_map_it->first + len - 1;
                            snp_indel_map_it++;
						}
                        else
                            pos2var_info.erase(snp_indel_map_it++);
					}
                    else
					{
                    	pos2var_info.erase(snp_indel_map_it++);
					}
                    break;
                case 1:
                    if (snp_indel_map_it->first + len < pos)
					{
                    	if (snp_indel_map_it->second.seq1 != "H")
						{
                        	outseq.append(seq, last_pos, snp_indel_map_it->first - 1 - last_pos);
							//test code
							tlen1 += snp_indel_map_it->first - 1 - last_pos;
							//test code//
                            snp_indel_map_it->second.seq2 = snp_indel_map_it->second.seq1;
                            snp_indel_map_it->second.length2 = snp_indel_map_it->second.length1;
                            last_pos = snp_indel_map_it->first + len - 1;
                            snp_indel_map_it++;
						}
                        else
                            pos2var_info.erase(snp_indel_map_it++);
					}
                    else
                    {
                    	pos2var_info.erase(snp_indel_map_it++);
					}
				}
			}
            else if (snp_indel_map_it->second.var_type == "snp")
			{
				switch (chain)
				{
                case 0:
					outseq.append(seq, last_pos, snp_indel_map_it->first - 1 - last_pos);
                    outseq.append(snp_indel_map_it->second.seq2);
					//test code
					tlen2 += snp_indel_map_it->first - last_pos;
					//test code//
                    snp_indel_map_it->second.seq1 = snp_indel_map_it->second.seq2;
                    last_pos = snp_indel_map_it->first;
                    ++snp_indel_map_it;
                    break;
                case 1:
                    outseq.append(seq, last_pos, snp_indel_map_it->first - 1 - last_pos);
                    outseq.append(snp_indel_map_it->second.seq1);
					//test code
					tlen1 += snp_indel_map_it->first - last_pos;
					//test code//
                    snp_indel_map_it->second.seq2 = snp_indel_map_it->second.seq1;
                    last_pos = snp_indel_map_it->first;
                    ++snp_indel_map_it;
				}
			}
            else 
			{
				cerr << "Error1: in small deletion allele" << endl;
			}
		}
        else
		{
        	outseq.append(seq.substr(last_pos, pos - 1 - last_pos));
			//test code
			if (chain == 0)
				tlen2 += pos - 1 - last_pos;
			else 
				tlen1 += pos - 1 - last_pos;
			//test code//
            last_pos = pos - 1;
            break;
        }
	}

    if (snp_indel_map_it == pos2var_info.end())
    {
    	outseq.append(seq, last_pos, pos - 1 - last_pos);
		//test code
		if (chain == 0)
			tlen2 += pos - 1 - last_pos;
		else 
			tlen1 += pos - 1 - last_pos;
		//test code//
        last_pos = pos - 1;
	}
	//test code
	total_len1 += tlen1;
	total_len2 += tlen2;
	cerr << tlen1 << '\t' << total_len1 << '\t' << tlen2 << '\t' << total_len2 << endl;
	//test code

}
/*
//Endogenous insertion
void ins_cp_simulation(const string &chr, const string& seq, map<int, sv_var_info>& pos2sv_info, map<string, map<int, int> >& chr2nbg2nend, map<pair<int, int>,map<pair<int, int>, pair<int, char> > >& pos2cppos_allele, char level)
{
	int cp_length, cp_pos, ins_pos;
    int seq_length = seq.length();
    int cp_times = rand() % kCopyTimes + 1;
    string subseq;
    while (1)
    {
   		cp_length = rand() % kIndelLenU + kIndelLenL;
        cp_pos = rand() % (seq_length - cp_length) + 1;
		if (cp_pos - kLargeflank < 0 || cp_pos + cp_length - 1 + kLargeflank > seq_length){continue;}
        if (!isInNArea(chr, cp_pos, cp_length, chr2nbg2nend) && isCanInsert(cp_pos, cp_length, pos2sv_info))
        {
			string subseq1, subseq2;
			map<int, var_info>::iterator bound_map_it = pos2var_info.lower_bound(cp_pos);
			int cp_pos_minus_1 = cp_pos - 1;
			sv_external_integration(seq , pos2var_info, bound_map_it , cp_pos_minus_1, cp_pos + cp_length - 1, subseq1, subseq2);

        	//subseq = seq.substr(cp_pos - 1, cp_length);
            cp_times = rand() % kCopyTimes + 1;
            //map<pair<begin_pos, end_pos>, allele>
            //allele == 0 ==> insert in allele 1
            //allele == 1 ==> insert in allele 2
            //allel3 == 2 ==> insert in both allele 1 and 2
			sv_var_info cp_var_info = {"copy", 3, subseq1, subseq2, cp_length, '.', level};
			pos2sv_info.insert(make_pair(cp_pos, cp_var_info));
            map<pair<int, int>, pair<int, char> > inspos2al;
            while (cp_times)
			{
            	ins_pos = rand() % seq_length + 1;
				if (ins_pos - kLargeflank < 0 || ins_pos + kLargeflank > seq_length){continue;}
                if (!isInNAreadForIns(chr, ins_pos, chr2nbg2nend) && isCanInsert(ins_pos, 1, pos2sv_info))
				{
                	int ho_he = rand() % 3;
                    //allele = 0,1,2. 0:allele 1 deletion,1:allele 2 deletion, 2 :both deletion
                    int allele;
                    if (ho_he == 0)
                    	allele = 2;
                    else
					{
                    	allele = rand() % 2;
					}
                    sv_var_info ins_var_info = {"lins", allele, subseq1, subseq2, cp_length, 'i', level};
                    pos2sv_info.insert(make_pair(ins_pos, ins_var_info));
                    inspos2al.insert(make_pair(make_pair(ins_pos, cp_length), make_pair(allele, level)));
                    --cp_times;
					//test
				}
			}
            //map<pair<int, int>,map<pair<int, int>, int > >&pos2cppos_allele
            pos2cppos_allele.insert(make_pair(make_pair(cp_pos, cp_pos + cp_length - 1), inspos2al));
            break;
		}
	}
}
*/
void inverse_seq(string& seq)
{
	char c;
    int l = seq.size();
    for(int i = 0; i < l / 2; i++)
	{
    	c = seq[i];
        switch (c)
        {
     	case 'A': c = 'T'; break;
        case 'a': c = 'T'; break;
        case 'T': c = 'A'; break;
        case 't': c = 'A'; break;
        case 'C': c = 'G'; break;
        case 'c': c = 'G'; break;
        case 'G': c = 'C'; break;
        case 'g': c = 'C'; break;
        case 'N': c = 'N'; break;
        case 'n': c = 'N';
        }
        seq[i] = seq[l-i-1];
        switch (seq[i])
        {
        case 'A': seq[i] = 'T'; break;
        case 'a': seq[i] = 'T'; break;
        case 'T': seq[i] = 'A'; break;
        case 't': seq[i] = 'A'; break;
        case 'C': seq[i] = 'G'; break;
        case 'c': seq[i] = 'G'; break;
        case 'G': seq[i] = 'C'; break;
        case 'g': seq[i] = 'C'; break;
      	case 'N': seq[i] = 'N'; break;
       	case 'n': seq[i] = 'N';
		}
        seq[l-i-1] = c;
	}
    if (l % 2 == 1)
    {
        switch (seq[l/2])
        {
        case 'A': seq[l/2] = 'T'; break;
        case 'a': seq[l/2] = 'T'; break;
        case 'T': seq[l/2] = 'A'; break;
        case 't': seq[l/2] = 'A'; break;
        case 'C': seq[l/2] = 'G'; break;
        case 'c': seq[l/2] = 'G'; break;
        case 'G': seq[l/2] = 'C'; break;
        case 'g': seq[l/2] = 'C'; break;
        case 'N': seq[l/2] = 'N'; break;
        case 'n': seq[l/2] = 'N';
        }
   } 
}
//judge whether the new variation position overlap with the N area,for deletion and inversion
bool isInNArea(int pos, int length, map<int, int>& nbg2nend)
{
	if (nbg2nend.empty())
    	return false;
    else
	{
        map<int, int>::iterator map_it = nbg2nend.begin();
        while (map_it != nbg2nend.end())
		{
        	// map_it->first - 1 > pos + length - 1, [gggggg]b[ggggggg]
            if (map_it->first >= pos && map_it->first <= pos + length - 1|| pos >= map_it->first && pos <= map_it->second)
     			return true;
            else if (map_it->first > pos + length - 1)
                return false;
            else
                ++map_it;
		}
        return false;
	}
}
//judge whether the insert position is in the N area
bool isInNAreadForIns(int pos, map<int, int>& nbg2nend)
{
	if (nbg2nend.empty())
        return false;
    else
    {
        map<int, int>::iterator map_it = nbg2nend.begin();
        while (map_it != nbg2nend.end())
    	{
            if (map_it->first > pos)
                return false;
            else if (map_it->first <= pos && pos < map_it->second)
                return true;
            else ++map_it;
        }
        return false;
	}
}

bool isInNArea(string chr, int pos, int length, map<string, map<int, int> >& chr2nbg2nend) 
{
	map<string, map<int, int> >::iterator map_it = chr2nbg2nend.find(chr);
	if (map_it == chr2nbg2nend.end()) {
		return false;
	}
	else {
		return isInNArea(pos, length, map_it->second);
	}
}

bool isInNAreadForIns(string chr, int pos, map<string, map<int, int> >& chr2nbg2nend)
{
	map<string, map<int, int> >::iterator map_it = chr2nbg2nend.find(chr);
	if (map_it == chr2nbg2nend.end()) {
		return false;
	}
	else {
		return isInNAreadForIns(pos, map_it->second);
	}


}
/*
//judge whether the new variation(deletion/inversion) position overlap with the olders
//can't include insertion point
bool isOverlap(int pos, int length, map<int, sv_var_info>& pos2sv_info)
{
        if (pos2sv_info.empty())
                return false;
        else
        {
                map<int, sv_var_info>::iterator map_it = pos2sv_info.begin();
                while (map_it != pos2sv_info.end())
                {
                        // map_it->first - 1 > pos + length - 1, [gggggg]b[ggggggg]

                        if (map_it->first > pos + length)
                                return false;
                        else if (map_it->second.var_type == "lins")
                        {
                                if (pos < map_it->first && map_it->first < pos + length)
                                        return true;
                                else
                                        ++map_it;
                        }
                        else if (map_it->first >= pos && map_it->first <= pos + length || pos >= map_it->first && pos <= map_it->first + map_it->second.length)
                                return true;
                        else
                               ++map_it;
                }
                return false;
        }
}

//judge whether the insert position is in the deletion or inversion area
bool isInVariationPos(int pos, map<int, sv_var_info>& pos2sv_info)
{
        if (pos2sv_info.empty())
                return false;
        else
        {
			map<int, sv_var_info>::iterator map_it = pos2sv_info.begin();
                while (map_it != pos2sv_info.end())
                {
                        if (map_it->first > pos)
                                return false;
                        else if (map_it->second.var_type == "lins")
                        {
                                if (map_it->first == pos)
                                        return true;
                                else
                                        ++map_it;
                        }
                        else if (map_it->first <= pos && pos < map_it->first + map_it->second.length)
                                return true;
                        else ++map_it;
                }
                return false;
        }
}
*/
int get_insert_length()
{
	int temp=rand() %100000+1;
	double ratio=double(temp)/100000;
	int m=0;
	for(int i=0;i<20;i++)
	{
		if(ratio<=indel_len_ratio[i]){m=i+1;break;}
	}
	return m;
}

string SNP_var(const char& base)
{
	int Snp_value=rand()%180+1;
	double n_ratio=double(Snp_value)/180;
	switch(base)
	{
	case 'A':
		for(int i=0;i<16;i++)
		{
			if(n_ratio<=A_ratio[i])
			{
				return snp_set[i];
			}
			if(i==15)
			{
//				#pragma omp critical
				cerr<<"the random number is error"<<endl;
				exit(EXIT_FAILURE);
			}
		}
		break;
	case 'T':
		for(int i=0;i<16;i++)
		{
			if(n_ratio<=T_ratio[i])
			{
				return snp_set[i];
			}
			if(i==15)
			{
			//	#pragma omp critical
				cerr<<"the random number is error"<<endl;
				exit(EXIT_FAILURE);
			}
		}
		break;
	case 'C':
		for(int i=0;i<16;i++)
		{
			if(n_ratio<=C_ratio[i])
			{
				return snp_set[i];
			}
			if(i==15)
			{
			//	#pragma omp critical
				cerr<<"the random number is error"<<endl;
				exit(EXIT_FAILURE);
			}
		}
		break;
	case 'G':
		for(int i=0;i<16;i++)
		{
			if(n_ratio<=G_ratio[i])
			{
				return snp_set[i];
			}
			if(i==15)
			{
			//	#pragma omp critical
				cerr<<"the random number is error"<<endl;
				exit(EXIT_FAILURE);
			}
		
		}
		break;
	default:

		cerr<< base << " the SNP reference base is not Clear."<<endl;
		exit(EXIT_FAILURE);
	}
}
void Small_variation(const string &chr, const string& hg_seq,const variation_ratio& var, map<string, map<int, var_info> >& chr2pos2var_info,char level)
{
	map<string, map<int, var_info> >::iterator chr2pos2var_info_it = chr2pos2var_info.find(chr);
	if (chr2pos2var_info_it == chr2pos2var_info.end()) {
		map<int, var_info> pos2var_info;
		chr2pos2var_info_it = chr2pos2var_info.insert(make_pair(chr, pos2var_info)).first;
	}


	if(level=='B')
	{
		if(VAR_type[1])//indel
		{
			indel_simulation(chr, hg_seq, var.b_indel_variation_ratio, chr2pos2var_info,level);
			//#pragma omp critical
			cout << "In Blood period:\tsmall indel simulation finished" << endl;
		}	
		if(VAR_type[0])//snp
		{
			snp_simulation(chr, hg_seq, var.b_snp_variation_ratio, chr2pos2var_info,level);
			//#pragma omp critical
			cout <<"In Blood period:\tsnp simulation finished "  << endl;
		}	
	}
	if(level=='P')
	{
		if(VAR_type[1])//indel
		{
			indel_simulation(chr, hg_seq, var.p_indel_variation_ratio, chr2pos2var_info,level);
			//#pragma omp critical
			cout <<"In Paracancerous period:\tsmall indel simulation finished" << endl;
		}	
		if(VAR_type[0])//snp
		{
			snp_simulation(chr, hg_seq, var.p_snp_variation_ratio, chr2pos2var_info,level);
			//#pragma omp critical
			cout <<"In Paracancerous period:\tsnp simulation finished "  << endl;
		}	
	}
	if(level=='C')
	{
		if(VAR_type[1])//indel
		{
			indel_simulation(chr, hg_seq, var.c_indel_variation_ratio, chr2pos2var_info,level);
			//#pragma omp critical
			cout <<"In Cancer period:\tsmall indel simulation finished" << endl;
		}	
		if(VAR_type[0])//snp
		{
			snp_simulation(chr, hg_seq, var.c_snp_variation_ratio, chr2pos2var_info,level);
			//#pragma omp critical
			cout <<"In Cancer period:\tsnp simulation finished "  << endl;
		}	
	}
}

void Long_variation(const string &chr, const string& hg_seq, const variation_ratio& var, map<string, string> &virus2seq, map<string, map<int, sv_var_info> >& chr2pos2sv_info, map<string, map<int, insertion_info> > &chr2pos2insertion_info, map<string, map<int, int> >& chr2nbg2nend, char level)
{	
	map<string, map<int, sv_var_info> >::iterator chr2pos2sv_info_it = chr2pos2sv_info.find(chr);
	if (chr2pos2sv_info_it == chr2pos2sv_info.end()) {
		map<int, sv_var_info> pos2sv_info;
		chr2pos2sv_info_it = chr2pos2sv_info.insert(make_pair(chr, pos2sv_info)).first;
	}

	map<string, map<int, insertion_info> >::iterator chr2pos2insertion_info_it = chr2pos2insertion_info.find(chr);
	if (chr2pos2insertion_info_it == chr2pos2insertion_info.end()) {
		map<int, insertion_info> pos2insertion_info;
		chr2pos2insertion_info_it = chr2pos2insertion_info.insert(make_pair(chr, pos2insertion_info)).first;
	}


	if(level=='B')
	{
		if(VAR_type[2])//long insertion
		{
			//insertion_simulation(chr, hg_seq, var.b_ins_variation_ratio, virus2seq, chr2pos2sv_info, chr2nbg2nend,pos2cppos_allele,level);
			del_copy_inv_simulation(chr, hg_seq, var.b_ins_variation_ratio, chr2pos2sv_info, chr2pos2insertion_info, chr2nbg2nend, "copy,translocation", level);
			//#pragma omp critical
			cout << "In Blood period:\tlarge insertion simulation finished " << endl;
		}
		if(VAR_type[3])//long deletion
		{
			//deletion_simulation(chr, hg_seq, var.b_del_variation_ratio, chr2pos2sv_info, chr2nbg2nend,pos2allele,level);
			del_copy_inv_simulation(chr, hg_seq, var.b_del_variation_ratio, chr2pos2sv_info, chr2pos2insertion_info, chr2nbg2nend, "ldel", level);
			//del_copy_inv_simulation(chr, hg_seq, var.b_del_variation_ratio * 0.25, chr2pos2sv_info, chr2pos2insertion_info, chr2nbg2nend, "del,translocation", level);
			//#pragma omp critical
			cout << "In Blood period:\tlarge deletion simulation finished " << endl;
		}
		if(VAR_type[4])//inversion
		{
			del_copy_inv_simulation(chr, hg_seq, var.b_inv_variation_ratio, chr2pos2sv_info, chr2pos2insertion_info, chr2nbg2nend, "inv", level);
			//#pragma omp critical
			cout << "In Blood period:\tlarge invertion simulation finished " << endl;
		}
	}
	if(level=='P')
	{
		if(VAR_type[2])//long insertion
		{
			del_copy_inv_simulation(chr, hg_seq, var.p_ins_variation_ratio, chr2pos2sv_info, chr2pos2insertion_info, chr2nbg2nend, "copy,translocation", level);
			//#pragma omp critical
			cout << "In Paracancerous period:\tlarge insertion simulation finished " << endl;
		}
		if(VAR_type[3])//long deletion
		{
			del_copy_inv_simulation(chr, hg_seq, var.p_del_variation_ratio, chr2pos2sv_info, chr2pos2insertion_info, chr2nbg2nend, "ldel", level);
			//del_copy_inv_simulation(chr, hg_seq, var.p_del_variation_ratio * 0.25, chr2pos2sv_info, chr2pos2insertion_info, chr2nbg2nend, "del,translocation", level);
			//#pragma omp critical
			cout << "In Paracancerous period:\tlarge deletion simulation finished " << endl;
		}
		if(VAR_type[4])//inversion
		{
			del_copy_inv_simulation(chr, hg_seq, var.p_inv_variation_ratio, chr2pos2sv_info, chr2pos2insertion_info, chr2nbg2nend, "inv", level);
			//#pragma omp critical
			cout << "In Paracancerous period:\tlarge invertion simulation finished "  << endl;
		}
	}
	if(level=='C')
	{
		if(VAR_type[2])//long insertion
		{
			del_copy_inv_simulation(chr, hg_seq, var.c_ins_variation_ratio, chr2pos2sv_info, chr2pos2insertion_info, chr2nbg2nend, "copy,translocation", level);
			//#pragma omp critical
			cout << "In Cancer period:\tlarge insertion simulation finished " << endl;
		}
		if(VAR_type[3])//long deletion
		{
			del_copy_inv_simulation(chr, hg_seq, var.c_del_variation_ratio * 0.75, chr2pos2sv_info, chr2pos2insertion_info, chr2nbg2nend, "ldel", level);
			del_copy_inv_simulation(chr, hg_seq, var.c_del_variation_ratio * 0.25, chr2pos2sv_info, chr2pos2insertion_info, chr2nbg2nend, "del,translocation", level);
			//#pragma omp critical
			cout << "In Cancer period:\tlarge deletion simulation finished "  << endl;
		}
		if(VAR_type[4])//inversion
		{
			del_copy_inv_simulation(chr, hg_seq, var.c_inv_variation_ratio, chr2pos2sv_info, chr2pos2insertion_info, chr2nbg2nend, "inv", level);
			//#pragma omp critical
			cout << "In Cancer period:\tlarge invertion simulation finished " << endl;
		}
	}
}

void Simulation_start(map<string, string> &chr2seq, vector<string> &vec_chr, map<string, string> &virus2seq, vector<string> &vec_virus, map<string, map<int, var_info> >& chr2pos2var_info, map<string, map<int, sv_var_info> >& chr2pos2sv_info, map<string, map<int, sv_var_info> >& virus2pos2sv_info, map<string, map<int, insertion_info> > &chr2pos2insertion_info, map<string, map<int, int> >& chr2nbg2nend, const variation_ratio& var, char level)
{
	//#pragma omp parallel sections
	//{
	string chr, hg_seq;
	for (int i = 0; i < vec_chr.size(); i++) {
		chr = vec_chr[i];
		hg_seq = chr2seq[chr];
		//#pragma omp section
		Small_variation(chr, hg_seq,var, chr2pos2var_info,level);
		//#pragma omp section
		Long_variation(chr, hg_seq, var, virus2seq, chr2pos2sv_info, chr2pos2insertion_info, chr2nbg2nend, level);

	}

	if (!virus2seq.empty()) {
		double variation_ratio;
		if (level == 'B') {
			variation_ratio = var.b_ins_variation_ratio * kVirusIntegrationRationInInsertion;
		}
		else if (level == 'P') {
			variation_ratio = var.p_ins_variation_ratio * kVirusIntegrationRationInInsertion;
		}
		else if (level == 'C') {
			variation_ratio = var.c_ins_variation_ratio * kVirusIntegrationRationInInsertion;
		}
		VirusIntegrationSimulation(chr2seq, virus2seq, vec_virus, chr2pos2insertion_info, chr2pos2sv_info, virus2pos2sv_info, chr2nbg2nend, variation_ratio, level);
	}

	if (VAR_type[2] == 1) {
		map<string, map<int, sv_var_info> >::iterator chr2pos2sv_info_it = chr2pos2sv_info.begin();
		while (chr2pos2sv_info_it != chr2pos2sv_info.end()) {
			map<int, sv_var_info>::iterator pos2sv_info_it = chr2pos2sv_info_it->second.begin();
			while (pos2sv_info_it != chr2pos2sv_info_it->second.end()) {
				if (pos2sv_info_it->second.var_type == "del,translocation") {
					insertion_simulation(chr2seq, vec_chr, chr2pos2insertion_info, chr2pos2sv_info, chr2nbg2nend, chr2pos2sv_info_it->first, pos2sv_info_it);
				}
				else if (pos2sv_info_it->second.var_type == "copy,translocation") {
					insertion_simulation(chr2seq, vec_chr, chr2pos2insertion_info, chr2pos2sv_info, chr2nbg2nend, chr2pos2sv_info_it->first, pos2sv_info_it);
				}
				++pos2sv_info_it;
			}
			++chr2pos2sv_info_it;
		}
	}
	ModifySmallVariation(chr2pos2var_info, chr2pos2insertion_info, chr2pos2sv_info, virus2pos2sv_info, chr2nbg2nend, level);

}


void Record_deletion(double mv_ratio,map<int,var_info>& pos2var_info, map<int, sv_var_info>& pos2sv_info,map<pair<int, int>, pair<int, char> >& pos2allele,map<pair<int, int>, map<pair<int, int>, pair<int, char> > >& pos2cppos_allele)
{//delete amount
	
	small_var_deletion(mv_ratio,pos2var_info);
	long_var_deletion(mv_ratio,pos2sv_info,pos2allele,pos2cppos_allele);
}


void small_var_deletion(double mv_ratio,map<int,var_info>& pos2var_info)
{
	vector<int> snp;
    vector<int> indel;
    map<int,var_info>::iterator iter1;
	double is_not=0.0;
	cout<<"before:small Variation number is"<<pos2var_info.size()<<endl;
    for(iter1=pos2var_info.begin();iter1!=pos2var_info.end();iter1++)
    {
        if(iter1->second.period=='P')
        {
		    is_not=rand()/double(RAND_MAX);
			if(is_not>mv_ratio)
			{
				if(iter1->second.var_type=="snp")
				{
					snp.push_back(iter1->first);
				}else //insertion or deletion
				{
					indel.push_back(iter1->first);
				}
			}
       	}
    }
    int snp_num=snp.size();
    int indel_num=indel.size();
    for(int i=0;i<snp.size();i++)
    {
    	pos2var_info.erase(snp.at(i));
    }
    for(int i=0;i<indel.size();i++)
    {
        pos2var_info.erase(indel.at(i));
    }
	snp.clear();
   	indel.clear();
	cout<<"after:small Variation number is "<<pos2var_info.size()<<endl;
}


void long_var_deletion(double mv_ratio,map<int, sv_var_info>& pos2sv_info,map<pair<int, int>,  pair<int, char> >& pos2allele,map<pair<int, int>, map<pair<int, int>,  pair<int, char> > >& pos2cppos_allele)
{
	vector<int> inv;
    vector<pair<int,int> > cnv_ins;
    vector<pair<int,int> > cnv_del;
    map<int,sv_var_info>::iterator iter2;
    map<pair<int, int>, pair<int, char> >::iterator iter3; 
	map<pair<int, int>, map<pair<int, int>, pair<int, char> > >::iterator iter4;
    vector<int>::iterator iterv;
	double is_not=0.0;
    //INVERSION
	cout<<"before:large Variation number is "<<pos2sv_info.size()<<endl;
    for(iter2=pos2sv_info.begin();iter2!=pos2sv_info.end();iter2++)
    {
		if(iter2->second.period=='P')
		{
			is_not=rand()/double(RAND_MAX);
			if(is_not>mv_ratio)
			{
				if(iter2->second.var_type=="inv")
                {
                	inv.push_back(iter2->first);
				}
			}
		}
	}
	int inv_num=inv.size();
    //CNV DELETION
    for(iter3=pos2allele.begin();iter3!=pos2allele.end();iter3++)
	{
		if(iter3->second.second=='P')
		{
			is_not=rand()/double(RAND_MAX);
			if(is_not>mv_ratio)
			{
				cnv_del.push_back(iter3->first);
			}
		}
	}
    int del_CNV_num=cnv_del.size();
   
    //CNV INSERTION
    for(iter4=pos2cppos_allele.begin();iter4!=pos2cppos_allele.end();iter4++)
	{
    	iter3=iter4->second.begin();
        if(iter3->second.second=='P')
		{
			is_not=rand()/double(RAND_MAX);
			if(is_not>mv_ratio)
			{
				cnv_ins.push_back(iter4->first);
			}
		}
	}
    int ins_CNV_num=cnv_ins.size();
//#pragma omp parallel for private(iter2)
    for(int i=0;i<inv.size();i++)
	{
        iter2=pos2sv_info.find(inv.at(i));
        pos2sv_info.erase(iter2);
	}
    inv.clear();
//#pragma omp parallel for private(iter3)
    for(int i=0;i<cnv_del.size();i++)
    {
    	iter3=pos2allele.find(cnv_del.at(i));
        pos2sv_info.erase(iter3->first.first);
        pos2allele.erase(iter3);
	}
    inv.clear();
//#pragma omp parallel for private(iter2,iter3,iter4)           
	for(int i=0;i<cnv_ins.size();i++)
    {
    	iter4=pos2cppos_allele.find(cnv_ins.at(i));
        for(iter3=iter4->second.begin();iter3!=iter4->second.end();iter3++)
		{
        	iter2=pos2sv_info.find(iter3->first.first);
            pos2sv_info.erase(iter2);
		}
           
        pos2cppos_allele.erase(iter4);
	}
    cnv_ins.clear();
    cout<<"after:large Variation number is "<<pos2sv_info.size()<<endl;
}

/*
void Output(ofstream &FOUT, ofstream &FOUTSNP, ofstream &FOUTINDEL, ofstream &FOUTSV, ofstream &FOUTSEQ, ofstream &FOUTCNV, string chr, string outseq1,string outseq2,map<int, var_info> pos2var_info,map<int, sv_var_info> pos2sv_info,map<pair<int, int>, pair<int, char> > pos2allele,map<pair<int, int>, map<pair<int, int>, pair<int, char> > > pos2cppos_allele)
{
	FOUT << ">" << chr << "/1" << endl;
	OutputSeq(FOUT, outseq1);
	FOUT << ">" << chr << "/2" << endl;
	OutputSeq(FOUT, outseq2);

	Output_small_info(pos2var_info, FOUTSNP, FOUTINDEL, chr);
	Output_large_info(pos2sv_info, pos2allele, pos2cppos_allele, FOUTSV, FOUTSEQ, FOUTCNV, chr);

}

*/

void Output_small_info(map<int, var_info>& pos2var_info, ofstream &FOUTSNP, ofstream &FOUTINDEL, string chr)
{
	map<int, var_info>::iterator map_it = pos2var_info.begin();
	if (VAR_type[0] == 1 && VAR_type[1] == 1)
	{

		int snp_homo_num = 0, snp_hete_num = 0, ins_homo_num = 0, ins_hete_num = 0, del_homo_num = 0, del_hete_num = 0;
		int length = 0, allele = 3;
		string seq;

		FOUTSNP << "chr" << '\t' << "pos" << '\t' << "ref" << '\t' << "allele1" << '\t' << "allele2" << '\t' << "period" << endl;
		FOUTINDEL << "type" << '\t' << "chr" << '\t' <<  "pos" << '\t' << "length" << '\t' << "seq" << '\t' << "allele(0,1,2)" << '\t' << "period" << endl;

		while (map_it != pos2var_info.end())
		{
			if (map_it->second.var_type == "snp")
			{
				FOUTSNP << chr << '\t' << map_it->first << '\t' << map_it->second.ref << '\t' << map_it->second.seq1 << '\t'
						<< map_it->second.seq2 << '\t' << map_it->second.period << endl;
				if (map_it->second.seq1 == map_it->second.seq2)
					++snp_homo_num;
				else
					++snp_hete_num;
			}
			else
			{
				length = map_it->second.length1 > map_it->second.length2 ? map_it->second.length1 : map_it->second.length2;
				if (map_it->second.seq1 == "H")
				{
					seq = map_it->second.seq2;
					allele = 1;
				}
				else
				{
					seq = map_it->second.seq1;
					if (map_it->second.seq2 == "H")
						allele = 0;
					else
						allele = 2;
				}
				if (map_it->second.var_type == "ins")
				{
					if (allele == 2)
						++ins_homo_num;
					else
						++ins_hete_num;
				}
				else
				{
					if (allele == 2)
						++del_homo_num;
					else
						++del_hete_num;
				}

				FOUTINDEL << map_it->second.var_type << '\t' << chr << '\t' << map_it->first << '\t' << length << '\t'
						  << seq << '\t' << allele << '\t' << map_it->second.period << endl;
			}
			++map_it;
		}
		FOUTSNP << "# " << chr << " snp stastics information:" << '\n'
		        << "# " << chr << " total snp number:" << snp_homo_num + snp_hete_num << '\n'
		        << "# " << chr << " homozygous snp number:" << snp_homo_num << '\n'
				<< "# " << chr << " heterozygous snp number:" << snp_hete_num << endl;
		FOUTINDEL << "# " << chr << " indel stastics information:" << '\n'
		          << "# " << chr << " total insertion number:" << ins_homo_num + ins_hete_num << '\n'
		          << "# " << chr << " homozygous insertion number:" << ins_homo_num << '\n'
			 	  << "# " << chr << " heterozygous insertion number:" << ins_hete_num << '\n' 
		          << "# " << chr << " total deletion number:" << del_homo_num + del_hete_num << '\n'
		          << "# " << chr << " homozygous deletion number:" << del_homo_num << '\n'
			 	  << "# " << chr << " heterozygous deletion number:" << del_hete_num << endl;
	}
	else if (VAR_type[0] == 1)
	{


		int snp_homo_num = 0, snp_hete_num = 0;

		FOUTSNP << "chr" << '\t' << "pos" << '\t' << "ref" << '\t' << "allele1" << '\t' << "allele2" << '\t' << "period" << endl;

		while (map_it != pos2var_info.end())
		{
			FOUTSNP << chr<< '\t' << map_it->first << '\t' << map_it->second.ref << '\t' << map_it->second.seq1 << '\t'
					<< map_it->second.seq2 << '\t' << map_it->second.period << endl;
			if (map_it->second.seq1 == map_it->second.seq2)
				++snp_homo_num;
			else
				++snp_hete_num;
			++map_it;
		}
		FOUTSNP << "# " << chr << " snp stastics information:" << '\n'
		        << "# " << chr << " total snp number:" << snp_homo_num + snp_hete_num << '\n'
		        << "# " << chr << " homozygous snp number:" << snp_homo_num << '\n'
				<< "# " << chr << " heterozygous snp number:" << snp_hete_num << endl;
	}
	else if (VAR_type[1] == 1)
	{


		int ins_homo_num = 0, ins_hete_num = 0, del_homo_num = 0, del_hete_num = 0;
		int length = 0, allele = 3;
		string seq;

		FOUTINDEL << "type" << '\t' << "chr" << '\t' <<  "pos" << '\t' << "length" << '\t' << "seq" << '\t' << "allele(0,1,2)" << '\t' << "period" << endl;


		while (map_it != pos2var_info.end())
		{
			length = map_it->second.length1 > map_it->second.length2 ? map_it->second.length1 : map_it->second.length2;
			if (map_it->second.seq1 == "H")
			{
				seq = map_it->second.seq2;
				allele = 1;
			}
			else
			{
				seq = map_it->second.seq1;
				if (map_it->second.seq2 == "H")
					allele = 0;
				else
					allele = 2;
			}
			if (map_it->second.var_type == "ins")
			{
				if (allele == 2)
					++ins_homo_num;
				else
					++ins_hete_num;
			}
			else
			{
				if (allele == 2)
					++del_homo_num;
					else
					++del_hete_num;
			}

			FOUTINDEL << map_it->second.var_type << '\t' << chr << '\t' << map_it->first << '\t' << length << '\t'
					  << seq << '\t' << allele << '\t' << map_it->second.period << endl;
			++map_it;
		}
		FOUTINDEL << "# " << chr << "indel stastics information:" << '\n'
		          << "# " << chr << "total insertion number:" << ins_homo_num + ins_hete_num << '\n'
		          << "# " << chr << "homozygous insertion number:" << ins_homo_num << '\n'
			 	  << "# " << chr << "heterozygous insertion number:" << ins_hete_num << '\n'
		          << "# " << chr << "total deletion number:" << del_homo_num + del_hete_num << '\n'
		          << "# " << chr << "homozygous deletion number:" << del_homo_num << '\n'
			 	  << "# " << chr << "heterozygous deletion number:" << del_hete_num << endl;
	}
}

void OutputDelInvInfo(ofstream &FOUTSV, map<int, sv_var_info>& pos2sv_info, string chr)
{
	if (!pos2sv_info.empty())
	{
		map<int, sv_var_info>::iterator map_it = pos2sv_info.begin();
		int ldel_homo_num = 0, ldel_hete_num = 0, inv_homo_num = 0, inv_hete_num = 0;

//		FOUTSV << "type" << '\t' << "chr" << '\t' << "pos" << '\t' << "length" << '\t' << "allele" << '\t' << "insType" << '\t' << "period" << endl;
		while (map_it != pos2sv_info.end()) {
			if (map_it->second.var_type == "ldel" || map_it->second.var_type == "inv") {
				FOUTSV << map_it->second.var_type << '\t' << chr << '\t' << map_it->first << '\t' << map_it->first + map_it->second.length - 1 << '\t' << map_it->second.allele << "\t.\t.\t.\t" << map_it->second.period << endl;

				if (map_it->second.var_type == "ldel") {
					if (map_it->second.allele == 2)
						ldel_homo_num++;
					else
						ldel_hete_num++;
				}
				else if (map_it->second.var_type == "inv") {
					if (map_it->second.allele == 2)
						inv_homo_num++;
					else
						inv_hete_num++;
				}
			}
			++map_it;
		}
		FOUTSV << "# " << chr << " sv stastics information:" << '\n'
		       << "# " << chr << " total large deletion number:" << ldel_homo_num + ldel_hete_num << '\n'
		       << "# " << chr << " homozygous large deletion number:" << ldel_homo_num << '\n'
			   << "# " << chr << " heterozygous large deletion number:" << ldel_hete_num << '\n' 
		       << "# " << chr << " total inversion number:" << inv_homo_num + inv_hete_num << '\n'
		       << "# " << chr << " homozygous inversion number:" << inv_homo_num << '\n'
			   << "# " << chr << " heterozygous inversion number:" << inv_hete_num << endl;
	}
	
}


void OutputInsTranInfo(ofstream &FOUTSV, map<int, insertion_info>& pos2insertion_info, string chr) 
{
	if (pos2insertion_info.empty()) { return; } 
	map<int, insertion_info>::iterator pos2insertion_info_it = pos2insertion_info.begin();
	map<int, sv_var_info>::iterator pos2sv_var_info_it;

	string chr_of_insertseq;
	while (pos2insertion_info_it != pos2insertion_info.end()) {
		pos2sv_var_info_it = pos2insertion_info_it->second.pos2sv_var_info_it;
		FOUTSV << pos2sv_var_info_it->second.var_type << '\t' <<  pos2insertion_info_it->second.chr << '\t' << pos2sv_var_info_it->first << '\t' << pos2sv_var_info_it->first + pos2sv_var_info_it->second.length - 1 << '\t' << pos2sv_var_info_it->second.allele << '\t' << chr << '\t' << pos2insertion_info_it->first - 1 << '\t' << pos2insertion_info_it->first << '\t' << pos2sv_var_info_it->second.period << endl;
		++pos2insertion_info_it;
	}
}

void OutputSeq(ofstream &FOUT, string seq)
{
	int row_q = seq.size() / 50;
	int tail_col = seq.size() % 50;
	for (int i = 0; i < row_q; i++)
		FOUT << seq.substr(i*50, 50) << endl;
	if (tail_col != 0)
		FOUT << seq.substr(row_q*50, tail_col) << endl;
}

void OutputFasta(ofstream &FOUT, string chr, string seq)
{
	FOUT << ">" << chr << endl;
	int row_q = seq.size() / 50;
	int tail_col = seq.size() % 50;
	for (int i = 0; i < row_q; i++)
		FOUT << seq.substr(i*50, 50) << endl;
	if (tail_col != 0)
		FOUT << seq.substr(row_q*50, tail_col) << endl;
}

/*
void Output_info(map<int, var_info>& pos2var_info,map<int, sv_var_info>& pos2sv_info,map<pair<int, int>, pair<int, char> >& pos2allele,map<pair<int, int>, map<pair<int, int>, pair<int, char> > >& pos2cppos_allele,string out_file)
{
	map<int, var_info>::iterator iter1;
	map<int, sv_var_info>::iterator iter2;
	map<pair<int, int>, pair<int, char> >::iterator iter3;
	map<pair<int, int>, map<pair<int, int>, pair<int, char> > >::iterator iter4;
	ofstream fout3;
	fout3.open(out_file.c_str());
	if(!fout3.is_open()){cout<<"output info file open failed!"<<endl;exit(EXIT_FAILURE);}
	fout3<<"Small: position\ttype\tperiod\tsequence1\tlength1\tsequence2\tlength2"<<endl;
	for(iter1=pos2var_info.begin();iter1!=pos2var_info.end();iter1++)
	{
		fout3<<iter1->first<<"\t"<<iter1->second.var_type<<"\t"<<iter1->second.period<<"\t"<<iter1->second.seq1<<"\t"<<iter1->second.length1<<"\t"<<iter1->second.seq2<<"\t"<<iter1->second.length2<<endl;
	}
	fout3<<"Long:position\ttype\tperiod\tallele type\tsequence\tlength"<<endl;
	for(iter2=pos2sv_info.begin();iter2!=pos2sv_info.end();iter2++)
	{
		fout3<<iter2->first<<"\t"<<iter2->second.var_type<<"\t"<<iter2->second.period<<"\t"<<iter2->second.allele<<"\t"<<iter2->second.seq<<"\t"<<iter2->second.length<<endl;
	}
	fout3<<"CNV deletion:period\tallele type(0:seq1 deletion,1:seq2 deletion,2:seq1 and seq2 are all deletion)\tref_start\tref_end"<<endl;
	for(iter3=pos2allele.begin();iter3!=pos2allele.end();iter3++)
	{
		fout3<<iter3->second.second<<"\t"<<iter3->second.first<<"\t"<<iter3->first.first<<"\t"<<iter3->first.second<<endl;
	}
	fout3<<"CNV insertion: ref_start\tref_end\tnumber"<<endl;
	fout3<<"period\tallele type(0:seq1 insertion,1:seq2 insertion,2:seq1 and seq2 are all insertion)\tins_ref_start\tins_length"<<endl;
	for(iter4=pos2cppos_allele.begin();iter4!=pos2cppos_allele.end();iter4++)
	{
		fout3<<iter4->first.first<<"\t"<<iter4->first.second<<"\t"<<iter4->second.size()<<endl;
		for(iter3=iter4->second.begin();iter3!=iter4->second.end();iter3++)
		{
			fout3<<iter3->second.second<<"\t"<<iter3->second.first<<"\t"<<iter3->first.first<<"\t"<<iter3->first.second<<endl;
		}
	}
	fout3.close();
}
*/

void display_usage()
{
	cout << "MVSC:a multi-variation simulator of cancer genome\nVersion " << VERSION << '\n'
		 << "Revision " << REVISION << '\n'
		 <<"Usage:\tmvsc\n"
		 <<"Required parameters:\n"
	 	 <<"\t-o\t<str>\toutput directory\n"
		 <<"\t-d\t<str>\tinput human reference\n\n"
			
	   	 <<"Optional:\n"
		 <<"\t-v\t<str>\tfive number string, it  represent in order[snp, small indels, large insersion, large deletion, inversion], 0 means Not simulate this type of variation,1 means simulation(default:11111)\n"
		 <<"\t-D\t<str>\tinput exogenous sequence for long indel(default:all of the large insertion are endogenous)\n"
		 <<"\t-n\t<str>\tinput the file which record N area(default:sv can happen in N area)\n"
		 <<"\t-b\t<double>\tthe variation ratio in blood(default:0.001)\n"
		 <<"\t-p\t<double>\tthe variation ratio in para based on blood(default:0.00001)\n"
		 <<"\t-c\t<double>\tthe variation ratio in cancer based on para(default:0.00003)\n"
			
		 <<"AUTHOR:\n"
		 <<"\tWriten by Qiu kunlong and Li dan\n"
		 <<"REPORTING BUGS:\n"
		 <<"\tqiukunlong@genomics.cn or lidan2@genomics.cn"<<endl;
}

bool isCanInsert(int pos, int len, map<int, var_info> &pos2var_info)
{
	if (pos2var_info.empty())
		return true;
	else
	{
		map<int, var_info>::iterator bound_map_it = pos2var_info.lower_bound(pos);
		if (bound_map_it == pos2var_info.end())
		{
			bound_map_it--;
			if (isSmallAndSmallOverlap(pos, len, bound_map_it))
				return false;
			else 
				return true;
		}
		else
		{
			if (isSmallAndSmallOverlap(pos, len, bound_map_it))
				return false;
			else
			{
				if (bound_map_it == pos2var_info.begin())
					return true;
				else
				{
					bound_map_it--;
					if ((isSmallAndSmallOverlap(pos, len, bound_map_it)))
						return false;
					else
						return true;
				}
			}
		}
	}
}

//for large variation
bool isCanInsert(int pos, int len, map<int, sv_var_info> &pos2sv_info)
{
	if (pos2sv_info.empty())
		return true;
	else
	{
		map<int, sv_var_info>::iterator bound_map_it = pos2sv_info.lower_bound(pos);
		if (bound_map_it == pos2sv_info.end())
		{
			bound_map_it--;
			if (isLargeAndLargeOverlap(pos, len, bound_map_it))
				return false;
			else 
				return true;
		}
		else
		{
			if (isLargeAndLargeOverlap(pos, len, bound_map_it))
				return false;
			else
			{
				if (bound_map_it == pos2sv_info.begin())
					return true;
				else
				{
					bound_map_it--;
					if ((isLargeAndLargeOverlap(pos, len, bound_map_it)))
						return false;
					else
						return true;
				}
			}
		}
	}
}

bool isCanInsert(string chr, int pos, int len, map<string, map<int, sv_var_info> > &chr2pos2sv_info)
{
	map<string, map<int, sv_var_info> >::iterator chr2pos2sv_info_it = chr2pos2sv_info.find(chr);

	if (chr2pos2sv_info_it == chr2pos2sv_info.end()) {
		return true;
	}
	else {
		return isCanInsert(pos, len, chr2pos2sv_info_it->second);
	}
}

bool isLocked(int pos, int len, map<int, insertion_info> &pos2insertion_info)
{
	map<int, insertion_info>::iterator lower_bound_it = pos2insertion_info.lower_bound(pos);
	int low = lower_bound_it->first - kLargeflank;
	int	up = lower_bound_it->first + kLargeflank;
	if (is2AreaOverlap(low, up, pos - kLargeflank, pos + len - 1 + kLargeflank))
		return true;
	else return false;
}

bool isSmallAndSmallOverlap(int pos, int len, map<int, var_info>::iterator bound_map_it)
{
	int low(0), up(0);
	
	if (bound_map_it->second.var_type == "snp" || bound_map_it->second.var_type == "ins")
	{
		low = bound_map_it->first - flank;
		up = bound_map_it->first + flank;
	}
	else if (bound_map_it->second.var_type == "del")
	{
		low = bound_map_it->first - flank;
		up = bound_map_it->first + (bound_map_it->second.length1 > bound_map_it->second.length2 ? bound_map_it->second.length1 : bound_map_it->second.length2) - 1 + flank;
	}
	else
	{
		cerr << "There are unkown variation types:" << bound_map_it->second.var_type <<  endl;
		return true;
	}

	if (is2AreaOverlap(low, up, pos - flank, pos + len - 1 + flank))
		return true;
	else return false;
}

//for large variation
bool isLargeAndLargeOverlap(int pos, int len, map<int, sv_var_info>::iterator bound_map_it)
{
	int low(0), up(0);
	
	if (bound_map_it->second.var_type == "lins")
	{
		low = bound_map_it->first - kLargeflank;
		up = bound_map_it->first + kLargeflank;
	}
	else if (bound_map_it->second.var_type == "ldel" || bound_map_it->second.var_type == "inv" || bound_map_it->second.var_type == "copy,translocation" || bound_map_it->second.var_type == "del,translocation")
	{
		low = bound_map_it->first - kLargeflank;
		up = bound_map_it->first + bound_map_it->second.length - 1 + kLargeflank;
	}
	else
	{
		cerr << "There are unkown variation types" << endl;
		return true;
	}

	if (is2AreaOverlap(low, up, pos - kLargeflank, pos + len - 1 + kLargeflank))
		return true;
	else return false;
}


bool is2AreaOverlap(int alow, int aup, int blow, int bup)
{
	if (blow >= alow && blow <= aup || alow >= blow && alow <= bup)
		return true;
	else return false;
}

int ReadVirusSeq(string file, map<string, string> &virus2seq, vector<string> &vec_virus)
{
	ifstream FIN(file.c_str());	
	if (!FIN) { cerr << "Open file virus reference file: " << file << " failure!" << endl; exit(1); }
	string chr_or_seq;
	string chr, seq;
	pair<map<string, string>::iterator, bool> ret;
	while (getline(FIN, chr_or_seq)) {
		if (chr_or_seq[0] == '>') {
			if (chr.empty()) {
				chr = chr_or_seq.substr(1);
				chr = GetStringBeforeSpace(chr);
			}
			else {
				ret = virus2seq.insert(make_pair(chr, seq));
				seq.clear();
				if (ret.second == 0) { cerr << "Warning: elememt " << chr << " already exists" << endl; }
				else { vec_virus.push_back(chr); }
			}
		}
		else {
			seq.append(chr_or_seq);
		}
	}
	ret = virus2seq.insert(make_pair(chr, seq));
	seq.clear();
	if (ret.second == 0) { cerr << "Warning: elememt " << chr << " already exists" << endl; }
	else { vec_virus.push_back(chr); }

	return 0;
}

string GetStringBeforeSpace(string str) {
	string str1; 
	for (int i = 0; i < str.length(); i++) {
		if (str[i] != ' ') {
			str1.append(1, str[i]);
		}
		else {
			break;
		}
	}
	return str1;
}
