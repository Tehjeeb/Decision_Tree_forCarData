#include<bits/stdc++.h>
using namespace std;
const int N=1728;
const int N8=(N*8)/10;
string Input[N+10],selected[N8+10][7],unselected[N-N8+10][7];///Input[i]+=",";
string decision[4]={"unacc", "acc", "good", "vgood"};

string attr[6][5]={
                    {"vhigh", "high", "med", "low"},
                    {"vhigh", "high", "med", "low"},
                    {"2", "3", "4", "5","more"},
                    {"2", "4","more"},
                    {"small", "med", "big"},
                    {"low", "med", "high"}
                    };
int attr_size[6]={4,4,5,3,3,3};
class Node
{
public:
    int atr_num;
    bool isLeaf;
    vector<Node>br;
};
uniform_real_distribution<double> unif(1,20000*N);
default_random_engine re;
int get_rand()
{
    return ((int)unif(re))%N;
}
void select_test_data()
{
    set<int>s;
    int i;
    for(i=0;i<N;i++)
        s.insert(i);
    for(i=0;i<N8;i++)
    {
        auto it=s.lower_bound(get_rand());
        if(it==s.end()) it--;
        int k,j=0,pos=0,len=0;///j will end up being 7.
        for(k=0;k<Input[*it].size();k++)
        {
            if(Input[*it][k]==',')
            {
                selected[i][j++]=Input[*it].substr(pos,len);
                pos=k+1,len=0;
            }
            else
                len++;
        }
        s.erase(it);
    }
    i=0;
    for(auto it=s.begin();it!=s.end();it++)
    {
        int k,j=0,pos=0,len=0;///j will end up being 7.
        for(k=0;k<Input[*it].size();k++)
        {
            if(Input[*it][k]==',')
            {
                unselected[i][j++]=Input[*it].substr(pos,len);
                pos=k+1,len=0;
            }
            else
                len++;
        }
        i++;
    }
    /*for(i=0;i<7;i++)
    {
        cout<<unselected[0][i]<<" ";
    }
    cout<<endl;*/
}
double calc_antropy(int a,int n,string s[][7])
{
    int cnt[attr_size[a]][4];
    int i,j,k;
    for(j=0;j<attr_size[a];j++)
        for(k=0;k<4;k++)
            cnt[j][k]=0;

    for(i=0;i<n;i++)
    {
        for(j=0;j<attr_size[a];j++)
        {
            if(s[i][a]==attr[a][j])
            {
                for(k=0;k<4;k++)
                {
                    if(s[i][6]==decision[k])
                    {
                        cnt[j][k]++;
                    }
                }
            }
        }
    }
    double antropy=0.00;
    for(i=0;i<attr_size[a];i++)
    {
        int tot=0;
        for(j=0;j<4;j++)
        {
            tot+=cnt[i][j];
        }
        double g=0.00;
        for(j=0;j<4;j++)
        {
            if(cnt[i][j]!=0)
                g+=((double)cnt[i][j]/(double)tot)*log2((double)tot/(double)cnt[i][j]);
        }
        antropy+=((double)tot/(double)n)*g;
    }

    return antropy;
}
Node Decision_tree_learning(int n,string s[][7],set<int> attributes)
{
    if(n==0)
    {
        cout<<"Nope"<<endl;
        Node node;
        node.atr_num=0;
        node.isLeaf=true;
        return node;
    }
    int i,j,x,k;
    ///is all same?
    int c[4]={0,0,0,0};
    for(i=0;i<n;i++)
    {
        for(j=0;j<4;j++)
        {
            if(decision[j]==s[i][6])
                c[j]++;
        }
    }
    int cnt=0;
    for(j=0;j<4;j++)
    {
        if(c[j]>0)
        {
            cnt++;
            x=j;
        }
    }
    if(cnt==1)
    {
        Node node;
        node.atr_num=x;
        node.isLeaf=true;
        //cout<<"ended"<<endl;
        return node;///all same

    }
    x=-1;
    double min_antropy=2.00;
    for(auto it=attributes.begin();it!=attributes.end();it++)
    {
        double atp=calc_antropy(*it,n,s);
        if(atp<min_antropy)
        {
            x=*it;
            min_antropy=atp;
        }
    }
    /*if(x==-1)
    cout<<n<<" "<<x<<endl;*/

    Node node;
    node.atr_num=x;
    node.isLeaf=false;
    attributes.erase(x);
    int nn;
    for(j=0;j<attr_size[x];j++)
    {
        nn=0;
        for(i=0;i<n;i++)
        {
            if(s[i][x]==attr[x][j])
            {
                nn++;
            }
        }
        string ss[nn][7];
        int z=0;
        for(i=0;i<n;i++)
        {
            if(s[i][x]==attr[x][j])
            {
                for(k=0;k<7;k++)
                {
                    ss[z][k]=s[i][k];
                }
                z++;
            }
        }
        if(nn!=0)
            node.br.push_back(Decision_tree_learning(nn,ss,attributes));
        else///plurality of parent
        {
            int mx=-1,y;
            for(i=0;i<4;i++)
            {
                if(mx<c[i])
                {
                    mx=c[i];
                    y=i;
                }
            }
            //cout<<y<<endl;
            Node nnode;
            nnode.atr_num=y;
            nnode.isLeaf=true;
            node.br.push_back(nnode);
        }
    }
    return node;
}
bool dfs(Node node,string s[7])
{
    if(node.isLeaf)
    {
        //cout<<decision[node.atr_num]<<" "<<s[6]<<endl;
        return (decision[node.atr_num]==s[6]);
    }
    int i;
    bool f=0;
    for(i=0;i<attr_size[node.atr_num];i++)
    {
        if(s[node.atr_num]==attr[node.atr_num][i])
        {
            f=f|dfs(node.br[i],s);
        }
    }
    return f;
}

int main()
{
    ifstream cin("cardata.txt");
    int i,j,k;
    for(i=0;i<N;i++)
    {
        getline(cin,Input[i]);
        Input[i]+=",";
    }
    cout<<"reading done"<<endl;
    double total_accuracy=0.0;
    vector<double>v;
    for(i=0;i<20;i++)
    {
        select_test_data();
        //cout<<"selected test data"<<endl;
        set<int>attributes_set;
        for(j=0;j<6;j++)
            attributes_set.insert(j);
        Node node=Decision_tree_learning(N8,selected,attributes_set);
        //cout<<"decision tree has been built"<<endl;
        int cnt=0;
        double accuracy;
        for(j=0;j<N-N8;j++)
        {
            if(dfs(node,unselected[j]))
                cnt++;
        }
        cout<<cnt<<" "<<N-N8<<endl;
        accuracy=((double)cnt/(double)(N-N8))*100.0;
        v.push_back(accuracy);
        total_accuracy+=accuracy;
    }
    total_accuracy/=20.0;
    double sigma;
    for(i=0;i<20;i++)
    {
        sigma+=(v[i]-total_accuracy)*(v[i]-total_accuracy);
    }
    sigma/=20.0;
    sigma=sqrt(sigma);
    cout<<"Accuracy after 20 iteration: "<<total_accuracy<<" standard deviation: "<<sigma<<endl;
}
