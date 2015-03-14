/**
 * @file token_p.cpp
 * @brief 字句解析器
 * @author Akira Otaka
 * @date 2015.03.14
 */
#include <iostream>
#include <fstream>    // ファイル処理用
#include <iomanip>    // 引数付きマニピュレータ用
#include <string>
#include <cstdlib>
#include <cctype>

using namespace std;

enum TknKind {
	/*'('     ')'     '+'     '-'     '*'     '/'*/
	Lparen=1, Rparen, Plus,   Minus,  Multi,  Divi,
	/*'='     ','     '"'*/
	Assign,   Comma,  DblQ,
	/*'=='    '!='    '<'     '<='    '>'     '>='*/
	Equal,    NotEq,  Less,   LessEq, Great,  GreatEq,

	If,       Else,   End,    Print,  Ident,  IntNum,
	String,   Letter, Digit,  EofTkn, Others,
	End_list
};

struct Token {
	TknKind kind;                                   // トークンの種類
	string  text;                                   // トークン文字列
	int     intVal;                                 // 数値定数のときの値

	//デフォルトコンストラクタ
	Token() {
		kind = Others;
		text = "";
		int Val = 0;
	}

	//初期化
	Token (TknKind k, const string& s, int d = 0) {
		kind = k;
		text = s;
		intVal = d;
	}
};

/*------------------------------*/
void initChTyp();
Token nextTkn();
int nextCh();
bool is_ope2(int c1, int c2);
TknKind get_kind(const string& s);

// 文字種表
TknKind ctyp[256];
// トークン格納
Token token;
// 入力ストリーム
ifstream fin;

// 字句
struct KeyWord {
	const char *keyName;
	// 種別
	TknKind keyKind;
};

KeyWord KeyWdTbl[] = {
	{"if",    If      },
	{"else",  Else    },
	{"end",   End     },
	{"print", Print   },
	{"(",     Lparen  },
	{")",     Rparen  },
	{"+",     Plus    },
	{"-",     Minus   },
	{"*",     Multi   },
	{"/",     Divi    },
	{"=",     Assign  },
	{",",     Comma   },
	{"==",    Equal   },
	{"!=",    NotEq   },
	{"<",     Less    },
	{"<=",    LessEq  },
	{">",     Great   },
	{">=",    GreatEq },
	{"",      End_list},
};
/*------------------------------*/

/**
 * @fn int main(int argc, char *argv[])
 * @breaf 字句解析結果を表示する
 * @param argc 引数の数
 * @param argv 引数の内容
 * @return 正常に終了したことを示す
 * @detail 字句解析結果を表示する
 */
int main(int argc, char *argv[]) {

	if (argc == 1) exit(1);
	fin.open(argv[1]); if (!fin) exit(1);

	cout << "text     kind intVal\n";
	initChTyp();
	for (token = nextTkn() ; token.kind != EofTkn; token = nextTkn()) {
		cout << left << setw(10) << token.text
			<< right << setw(3) << token.kind
			<< " " << token.intVal << endl;
	}

	return 0;
}

/*
 * 文字種表設定
 */
void initChTyp() {
	int i;

	for (i=0; i<256; i++)   { ctyp[i] = Others; }
	for (i='0'; i<'9'; i++)   { ctyp[i] = Digit; }
	for (i='A'; i<'Z'; i++)   { ctyp[i] = Letter; }
	for (i='a'; i<'z'; i++)   { ctyp[i] = Letter; }
	ctyp['('] = Lparen;
	ctyp[')'] = Rparen;
	ctyp['<'] = Less;
	ctyp['>'] = Great;
	ctyp['+'] = Plus;
	ctyp['-'] = Minus;
	ctyp['*'] = Multi;
	ctyp['/'] = Divi;
	ctyp['_'] = Letter;
	ctyp['='] = Assign;
	ctyp[','] = Comma;
	ctyp['"'] = DblQ;

	return;
}

/**
 * @fn Token nextTkn()
 * @breaf トークン（字句列）取得する
 * @return トークンをその情報と共に返す
 * @detail
 * 字句を一つ取得し、その字句を元に種類を特定し、字句列を取り出して返す
 * なお、漢字やエスケープ文字は非対応
 */
Token nextTkn() {
	TknKind kd;
	int ch0;
	int num = 0;

	// staticで前回文字を保持
	static int ch = ' ';
	string txt = "";

	// 空白読み捨て
	while (isspace(ch)) {
		ch = nextCh();
	}

	// ファイルの末端
	if (ch == EOF) {
		return Token(EofTkn, txt);
	}

	switch (ctyp[ch]) {
		case Letter:  /* 識別子 */
			for (; ctyp[ch]==Letter || ctyp[ch]==Digit; ch=nextCh()) {
				txt += ch;
			}
			break;
		case Digit:   /* 数字 */
			for (num=0; ctyp[ch]==Digit; ch=nextCh()) {
				num = num * 10 + (ch - '0');
			}
			return Token(IntNum, txt, num);
		case DblQ:
			for (ch = nextCh(); ch != EOF && ch != '\n' && ch != '"'; ch = nextCh()) {
				txt += ch;
			}
			if (ch != '"') {
				cout << "文字列リテラルが閉じていない\n";
				exit (1);
			}
			ch = nextCh();
			return Token(String, txt);
		default:
			txt += ch;
			ch0 = ch;
			ch = nextCh();
			if (is_ope2 (ch0, ch)) {
				txt += ch;
				ch = nextCh();
			}
	}

	kd = get_kind(txt);
	if (kd == Others) {
		cout << "不正なトークンです: " << txt << endl; exit(1);
	}

	return Token(kd, txt);
}

/**
 * @fn int nextCh()
 * @breaf 次の字句を取得する
 * @return 取得した字句を返す
 * @detail 前回取得した字句がファイル終端であれば終了し、それ以外の場合は取得した字句を返す
 */
int nextCh() {
	static int c = 0;
	if (c == EOF) {
		return c;
	}

	// 終了
	if ((c = fin.get()) == EOF) {
		fin.close();
	}
	return c;
}

/**
 * @fn bool is_ope2 (int c1, int c2)
 * @breaf 2文字演算であるかを評価する
 * @param c1 演算子1
 * @param c2 演算子2
 * @return 2文字演算なら真、異なるなら偽を返す
 * @detail 2つの文字列を結合し、予め定められた2文字演算と一致するか否かを評価する
 */
bool is_ope2 (int c1, int c2) {
	char s[] = "    ";
	if (c1  == '\0' || c2 == '\0') {
		return false;
	}
	s[1] = c1;
	s[2] = c2;
	return strstr(" <= >= == != ", s) != NULL;
}

/**
 * @fn TknKind get_kind(const string& s)
 * @breaf トークンを引数とし、その種類を返す
 * @param s トークン
 * @return トークンの種類
 * @detail トークンの種類は、予約語、文字列、整数列、その他の順で評価される
 */
TknKind get_kind(const string& s) {
	//予約語との完全一致を評価する
	for (int i = 0; KeyWdTbl[i].keyKind != End_list; i++) {
		if (s == KeyWdTbl[i].keyName) {
			return KeyWdTbl[i].keyKind;
		}
	}

	//予約語でない場合は文字列であるかを評価する
	//(先頭がアルファベットの場合はすべて文字列として良い)
	if (ctyp[s[0]] == Letter) return Ident;
	//文字列でない場合は性数列であるかを評価する
	//(先頭が数字の場合はアルファベットは混在しない)
	if (ctyp[s[0]] == Digit)  return IntNum;

	//いずれも当てはまらない場合はその他と評価する
	return Others;
}




