//Kyokai no Kanata //
#include<bits/stdc++.h>

using namespace std;

typedef pair<int,int> II;
typedef vector<II> VII;
typedef vector<int> VI;
typedef vector< VI > VVI;
typedef long long int LL;

#define PB push_back
#define MP make_pair
#define F first
#define S second
#define SZ(a) (int)(a.size())
#define ALL(a) a.begin(),a.end()
#define SET(a,b) memset(a,b,sizeof(a))

#define si(n) scanf("%d",&n)
#define dout(n) printf("%d\n",n)
#define sll(n) scanf("%lld",&n)
#define lldout(n) printf("%lld\n",n)
#define fast_io ios_base::sync_with_stdio(false);cin.tie(NULL)
#define TRACE

#ifdef TRACE
#define trace(...) __f(#__VA_ARGS__, __VA_ARGS__)
template <typename Arg1>
void __f(const char* name, Arg1&& arg1){
  cerr<<name<<" : "<<arg1<<endl;
}
template <typename Arg1, typename... Args>
void __f(const char* names,Arg1&& arg1,Args&&... args){
  const char* comma=strchr(names+1,',');
  cerr.write(names,comma-names)<<" : "<<arg1<<" | ";__f(comma+1,args...);
}
#else
#define trace(...)
#endif
const int LLIMIT = -int(2e9),RLIMIT = int(2e9);
class Node;
class InternalNode;
class LeafNode;
InternalNode *Root;
class Node{
  public:
    int M;vector <int> Keys;
    InternalNode *parent;
    Node(int m,InternalNode *PP=NULL):M(m),parent(PP){}
    virtual void insert(int key,Node *pnt,int psz,int psz2)=0;
    virtual pair<LeafNode*,int> query(int leftkey,int rightkey)=0;
    virtual void print()=0;
};
class InternalNode:public Node{
  public:
    bool root;vector <Node*> pointer;
    vector <int> sizes;int sz;
    InternalNode(int m,bool fl,InternalNode* PP=NULL):Node(m,PP),sz(0),root(fl){}
    void split(){
      InternalNode* newNode = new InternalNode(M,0,parent);
      for(int i=ceil((M+1)/2.0);i<Keys.size();i++) newNode->Keys.push_back(Keys[i]);
      for(int i=ceil((M+1)/2.0);i<pointer.size();i++) newNode->pointer.push_back(pointer[i]),pointer[i]->parent = newNode;
      for(int i=ceil((M+1)/2.0);i<pointer.size();i++) newNode->sizes.push_back(sizes[i]),newNode->sz+=sizes[i];
      sz -= newNode->sz;
      if(root){
        root =0;
        Root = new InternalNode(M,1);
        Root->Keys.push_back(Keys[ceil((M-1)/2.0)]);
        Root->pointer.push_back(this);Root->pointer.push_back(newNode);
        Root->sizes.push_back(sz);Root->sizes.push_back(newNode->sz);
        Root->sz+=sz+newNode->sz;
        parent = Root,newNode->parent=Root;
      }
      else  parent->insert(Keys[ceil((M-1)/2.0)],newNode,newNode->sz,sz);
      Keys.resize(ceil((M-1)/2.0));
      pointer.resize(ceil((M+1)/2.0));
      sizes.resize(ceil((M+1)/2.0));
    }
    void insert(int key,Node *pnt,int psz,int psz2){
      int x =  lower_bound(ALL(Keys),key) - Keys.begin();
      int pntpos = x+1;
      //trace(key,psz);
      Keys.push_back(key);pointer.push_back(pnt),sizes.push_back(psz);
      for(int i=pointer.size()-1;i>pntpos && i-1;i--) swap(Keys[i-1],Keys[i-2]),swap(pointer[i],pointer[i-1]),swap(sizes[i],sizes[i-1]);
      sizes[pntpos-1] = psz2;
      if(pointer.size()==M+1) split();
    }
    void updatesize(int key){
      sz++; 
      int p =  lower_bound(ALL(Keys),key) - Keys.begin(); 
      sizes[p]++;
      if(root) return;
      parent->updatesize(key);
    }
    pair<LeafNode*,int>  query(int x,int y)
    {
      //     trace("query",x,y);
      int lp =  lower_bound(ALL(Keys),x) - Keys.begin();
      int rp =  upper_bound(ALL(Keys),y) - Keys.begin();
      pair <LeafNode*,int> lret ,rret;
      if(lp!=rp)
        lret = pointer[lp]->query(x,y),rret = pointer[rp]->query(x,y),lret.S+=rret.S;
      else lret = pointer[lp]->query(x,y);
      int sum=0;
      //   for(int i=0;i<sizes.size();i++)  trace(i,sizes[i]);
      for(int i=lp+1;i<rp;i++) sum+=sizes[i];
      //   trace(sum,lret.S);
      return MP(lret.F,sum+lret.S);       
    }
    void print(){
      for(int i=0;i<Keys.size();i++) printf("%d :%d: ",sizes[i],Keys[i]);
      printf("%d\n",sizes[Keys.size()]) ;
      for(auto pnt:pointer) pnt->print();    
    }
};
class LeafNode:public Node{
  public:
    LeafNode(int m,InternalNode *PP =NULL):Node(m,PP){}
    void split(){
      LeafNode *newNode  = new LeafNode(M,parent);
      for(int i=ceil((M+1)/2.0);i<Keys.size();i++) newNode->Keys.push_back(Keys[i]);
      // trace(newNode->Keys.size());
      int key = Keys[ceil((M+1)/2.0)];
      Keys.resize(ceil((M+1)/2.0));
      if(!parent){
        Root = new InternalNode(M,1);
        Root->Keys.push_back(key);
        Root->pointer.push_back(this);Root->pointer.push_back(newNode);
        Root->sizes.push_back(Keys.size());Root->sizes.push_back(newNode->Keys.size());
        Root->sz+= Keys.size()+newNode->Keys.size();
        parent = Root,newNode->parent=Root;
      }
      else
        parent->insert(key,newNode,newNode->Keys.size(),Keys.size());
    }
    void insert(int key,Node *pnt=NULL,int ssz=1,int psz=1){
      int x =  lower_bound(ALL(Keys),key) - Keys.begin();
      //  trace(x);
      Keys.push_back(key);
      for(int i=Keys.size()-1;i>x;i--) swap(Keys[i],Keys[i-1]);
      updatesize(key);
      if(Keys.size()==M+1) split();
    }
    void updatesize(int key){
      if(parent) parent->updatesize(key);
    }
    pair<LeafNode*,int>  query(int x,int y)
    {
      // trace("leafquery",x,y);
      int lp =  lower_bound(ALL(Keys),x) - Keys.begin();
      int rp =  upper_bound(ALL(Keys),y) - Keys.begin();
      return MP(this,rp-lp);
    }     
    void print(){
      for(auto key:Keys) printf("%d ",key);  
      printf("\n") ;
    }
};
class Bplus{
  public:
    int M;
    LeafNode *leftLeaf;
    Bplus(int m) {Root=NULL,M=m,leftLeaf = new LeafNode(M);}
    pair<LeafNode*,int> query(int x,int y){
      if(Root) return Root->query(x,y);
      else return leftLeaf->query(x,y); 
    }
    void insert(int x){
      auto ret = query(x,x);
      ret.F->insert(x);
    }
    void print(){
      if(Root) return Root->print();
      else return leftLeaf->print(); 
    } 
};
char C[500];
vector<pair<string,pair<int,int>>> Input;
vector<pair<string,int>> Output;
void emptyOutput()
{
      reverse(ALL(Output));
      while(Output.size())
      {
        auto x = Output.back(); 
        Output.pop_back(); 
        if(x.F.size()) cout<<x.F<<"\n";
        else dout(x.S);
      }
      fflush(stdout);
}
int main(int argc,char *argv[])
{
  if(argc<4) cerr<<"Invalid input" <<endl;
  int M = atoi(argv[1]); 
  int B = atoi(argv[2]);freopen(argv[3],"r",stdin);
 // trace(M,B,argv[3]);
  Bplus T = Bplus(501);
  int x,y;
  while(1)
  {
    while(Input.size()<((M-1)*B) && scanf("%s",C)!=EOF){
      string s=string(C);
      switch(s[0]){
        case 'I':
          si(x); Input.PB({s,{x,0}});
          break;
        case 'F':
          si(x); Input.PB({s,{x,0}});
          break;
        case 'C':
          si(x); Input.PB({s,{x,0}});
          break;
        case 'R':
          si(x);si(y); Input.PB({s,{x,y}});
          break;
        default:
          trace(s[0]);
          cerr<<"Unknown Input"<<endl;
      }            
    }
    if(!Input.size()) {emptyOutput();break;}
    reverse(ALL(Input)); 
    while(Input.size())
    {
      auto x = Input.back(); 
      Input.pop_back(); 
      switch(x.F[0]){
        case 'I':
          T.insert(x.S.F);
          break;
        case 'F':
          if(T.query(x.S.F,x.S.F).S) Output.PB({"YES",0});
          else Output.PB({"NO",0});
          break;
        case 'C':
          Output.PB({"",T.query(x.S.F,x.S.F).S});
          break;
        default:
          Output.PB({"",T.query(x.S.F,x.S.S).S});
      }
//      trace(Input.size(),Output.size());
      if(Output.size()==B) emptyOutput();
 //     trace(Input.size(),Output.size());
    }
  }
  return 0;
}
