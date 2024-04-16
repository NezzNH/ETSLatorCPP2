#include <iostream>
#include <string>
#include <stack>
#include <cmath>
#include <queue>
#include <iomanip>

using namespace std;

typedef unsigned short us;

/*
I need a custom parser for special functions. I hope I can consolidate it all into one simple function.
Then, I'll do checks on the expression itself. If I find that one of the exponents is larger than 2 for example in the square equation case,
I'll just return an error. Similarly so for other functions

I also have to implement rejection of invalid arithmetic and geometric series. Shouldn't be too difficult, just a bit of a pain in the ass.
Also gotta implement a series constructor for the inverse of analysis of a series.
Lastly is to implement all of the analytical geometry eqs, those should do just fine with the same universal complex expression parser.
Rejection of invalid expressions is going to be a bit more complicated but I'll try that until Friday. I have plenty of time, but also shit to do so idk.
*/

const static int OPERATOR_ARR_SIZE = 10;
const static int EXP_PRO_SIZE = 4;
const static int EXPRCORRSIZE = 12;
const static int RESULT_DISP_PRECISION = 2;

enum class tokenType { OPERATOR, OPERAND, NIL }; //
enum class associativity { LEFT, RIGHT };
enum class ExpressionType { SQUARE_EQ, ARITHMETICARR, GEOMETRICARR, ELLIPSE, SPHERE, HYPERBOLA, LOGICAL, PARABOLA, NORMAL, ERR };
enum class OPERATOR_TYPE { ADD, SUBTRACT, MULTIPLY, DIVIDE, NIL, POWER, MODULUS, UNARY_MINUS };

string SPECIAL_TOKENS[6] = { "sin", "cos", "tg", "ctg", "max", "min" };

const static char OPERATORS_CHAR[OPERATOR_ARR_SIZE] = { '+', '-', '/', '*', '=', '(', ')', '^', '%', 'u' };

string ExpressionPrototypesArray[EXP_PRO_SIZE] = {
	"x^2 o x o n",
	"x^2/p + y^2/p = p",
	"(x-n)^2 + (y-n)^2 = p",
	"x^2/p - y^2/p = p",
};

class Token {
private:
	tokenType tt;
	string contents;
	us precedence;
	associativity as;
public:
	Token() {
		tt = tokenType::NIL;
		contents = "";
		precedence = 0;
		as = associativity::LEFT;
	}
	Token(tokenType tt, string contents, us precedence, associativity as = associativity::LEFT) {
		this->tt = tt;
		this->contents = contents;
		this->precedence = precedence;
		this->as = as;
	}
	double returnValue() {
		double value = stod(contents);
		return value;
	}
	string returnContents() { return contents; }
	us returnPrecedence() { return precedence; }
	tokenType returnTokenType() { return tt; }
	associativity returnAss() { return as; }

	friend ostream& operator<<(ostream& os, const Token& tk);
};

ostream& operator<<(ostream& os, const Token& tk) {
	os << "T: ";
	if (tk.tt == tokenType::OPERAND) os << "oprnd ";
	else os << "oprtr ";
	os << tk.contents << " " << tk.precedence << " ";
	if (tk.as == associativity::LEFT) os << "left" << endl;
	else os << "right" << endl;

	return os;
}

tokenType returnTokenType(string expression) {
	if (expression == "" || expression == " ") return tokenType::NIL;
	if (expression.length() > 1) return tokenType::OPERAND;
	for (int i = 0; i < OPERATOR_ARR_SIZE; i++) {
		if (expression[0] == OPERATORS_CHAR[i]) return tokenType::OPERATOR;
	}
	return tokenType::OPERAND;
}

int returnAmountOfTokens(string expression) {
	int result = 0;
	expression += ' ';
	string toStr = "", previousBuffer = "";
	for (int i = 0; i < expression.length(); i++) {
		toStr = expression[i];
		if (returnTokenType(toStr) == tokenType::OPERATOR || toStr == " ") {
			if (returnTokenType(previousBuffer) == tokenType::OPERAND) result += 2;
			else result++;
		}
		previousBuffer = toStr;
	}
	return result - 1;
}

us returnPrecedence(char op) {
	switch (op) {
	case '+':
		return 2;
	case '-':
		return 2;
	case '*':
		return 3;
	case '/':
		return 3;
	case '^':
		return 4;
	case '%':
		return 3;
	case 'u':
		return 4;
	default:
		return 0;
	}
}

associativity identifyAss(char op) {
	if (op == '^') return associativity::RIGHT;
	else return associativity::LEFT;
}

OPERATOR_TYPE returnOpType(char opSymbol) {
	switch (opSymbol) {
	case '+':
		return OPERATOR_TYPE::ADD;
	case '-':
		return OPERATOR_TYPE::SUBTRACT;
	case '/':
		return OPERATOR_TYPE::DIVIDE;
	case '*':
		return OPERATOR_TYPE::MULTIPLY;
	case '^':
		return OPERATOR_TYPE::POWER;
	case '%':
		return OPERATOR_TYPE::MODULUS;
	case 'u':
		return OPERATOR_TYPE::UNARY_MINUS;
	default:
		return OPERATOR_TYPE::NIL;
	}
}

double evaluateRPN(Token tokenArray[], int tokenArraySize) {

	stack<double> evaluationStack;
	double op1, op2, result, resultTotal = 0;

	for (int i = 0; i < tokenArraySize; i++) {
		Token currentToken = *tokenArray;
		if (currentToken.returnTokenType() == tokenType::OPERAND) {
			evaluationStack.push(currentToken.returnValue());
		}
		else {
			char opSymbol = currentToken.returnContents()[0];
			OPERATOR_TYPE opType = returnOpType(opSymbol);
			op1 = evaluationStack.top();
			evaluationStack.pop();
			if (opType != OPERATOR_TYPE::UNARY_MINUS) {
				if (evaluationStack.empty()) {
					op2 = 0;
				}
				else {
					op2 = evaluationStack.top();
					evaluationStack.pop();
				}
				switch (opType) {
				case OPERATOR_TYPE::ADD:
					result = op1 + op2;
					break;
				case OPERATOR_TYPE::SUBTRACT:
					result = op2 - op1;
					break;
				case OPERATOR_TYPE::MULTIPLY:
					result = op1 * op2;
					break;
				case OPERATOR_TYPE::DIVIDE:
					if (op1 == 0) result = 0;
					else result = op2 / op1;
					break;
				case OPERATOR_TYPE::POWER:
					result = pow(op2, op1);
					break;
				case OPERATOR_TYPE::MODULUS:
					result = ((int)op2 % (int)op1);
					break;
				default:
					result = 0;
					break;
				}
			}
			else {
				result = 0 - op1;
			}
			op1 = 0;
			op2 = 0;
			evaluationStack.push(result);
		}
		tokenArray++;
	}

	resultTotal = evaluationStack.top();
	evaluationStack.pop();
	return resultTotal;
}

void splitupExpr(string expression, string result[], int arraySize) {
	expression += ' ';
	bool unary = false;
	string toStr, buffer = "", previousbuffer = "";
	for (int i = 0; i < expression.length(); i++) {
		unary = false;
		toStr = expression[i];
		if (returnTokenType(toStr) == tokenType::OPERATOR || toStr == " ") {	//gotta recognize unary '-'. probably just checking 3 different conditions when finding a - and assigning u to it
			if (i == 0 && expression[i] == '-') unary = true;
			else if (returnTokenType(previousbuffer) == tokenType::OPERATOR && expression[i] == '-') unary = true;	//yes this repeats. issues on vs code chaining conditionals for some fucking reason
			if (returnTokenType(buffer) != tokenType::NIL) {
				*result = buffer;
				result++;
				previousbuffer = buffer;
				buffer = "";
			}
			if (toStr != " ") {
				if (unary) *result = 'u';
				else *result = toStr;
				result++;
			}
		}
		else {
			buffer += expression[i];
		}
	}
}

string deSpaceify(string input) {
	string result = "";
	for (int i = 0; i < input.length(); i++) {
		if (input[i] != ' ') result += input[i];
	}
	return result;
}

void shoveIntoTokens(Token tokenArray[], string preTokenArray[], int arraySize) {
	for (int i = 0; i < arraySize; i++) {
		tokenType currentTokenType = returnTokenType(*preTokenArray);
		string buffer = *preTokenArray;
		us currentPrecedence = returnPrecedence(buffer[0]);
		associativity tempAssociativity = identifyAss(buffer[0]);
		*tokenArray = Token(currentTokenType, *preTokenArray, currentPrecedence, tempAssociativity);
		tokenArray++;
		preTokenArray++;
	}
}

bool shuntingYard(Token tokenArray[], int tokenArraySize, Token resultArray[], int* finalArraySize) {
	stack<Token> operatorStack;
	queue<Token> outputQueue;
	for (int i = 0; i < tokenArraySize; i++) {
		Token currentToken = *tokenArray;
		if (currentToken.returnTokenType() == tokenType::OPERAND) outputQueue.push(currentToken);
		else {
			if (currentToken.returnContents()[0] != '(' && currentToken.returnContents()[0] != ')') {
				while (!operatorStack.empty()
					&& operatorStack.top().returnContents()[0] != '('
					&& (operatorStack.top().returnPrecedence() > currentToken.returnPrecedence()
					|| (currentToken.returnPrecedence() == operatorStack.top().returnPrecedence() && currentToken.returnAss() == associativity::LEFT)))
				{
					outputQueue.push(operatorStack.top());
					operatorStack.pop();
				}
				operatorStack.push(currentToken);
			}
			else if (currentToken.returnContents()[0] == '(') {
				operatorStack.push(currentToken);
			}
			else if (currentToken.returnContents()[0] == ')') {
				while (operatorStack.top().returnContents()[0] != '(') {
					if (operatorStack.empty()) {
						cout << "Mismatched parentheses!";
						return false;
					}
					outputQueue.push(operatorStack.top());
					operatorStack.pop();
				}
				if (operatorStack.top().returnContents()[0] != '(') {
					cout << "Mismatched parentheses!";
					return false;
				}
				else operatorStack.pop();

			}
		}
		tokenArray++;
	}

	while (!operatorStack.empty()) {
		if (operatorStack.top().returnContents()[0] == '(' || operatorStack.top().returnContents()[0] == ')') {
			cout << "Mismatched parentheses!";
			return false;
		}
		outputQueue.push(operatorStack.top());
		operatorStack.pop();
	}

	while (!outputQueue.empty()) {
		*finalArraySize = *finalArraySize + 1;
		*resultArray = outputQueue.front();
		resultArray++;
		outputQueue.pop();
	}
	return true;
}

double EvalNormalEq(string expression) {

	int tokenArraySize = 0, resultArraySize = 0, * resultArraySizeP = &resultArraySize;
	tokenArraySize = returnAmountOfTokens(expression);
	string* preTokenArray = new string[tokenArraySize];

	splitupExpr(expression, preTokenArray, tokenArraySize);

	Token* tokensArray = new Token[tokenArraySize];

	shoveIntoTokens(tokensArray, preTokenArray, tokenArraySize);

	delete[] preTokenArray;

	Token* shuntedArray = new Token[tokenArraySize];

	bool success = false;

	success = shuntingYard(tokensArray, tokenArraySize, shuntedArray, resultArraySizeP);

	if (!success) cout << "Something went wrong!";

	double result = evaluateRPN(shuntedArray, resultArraySize);

	delete[] tokensArray;
	delete[] shuntedArray;

	return result;
}

string EvalSeries(string expression, bool IsArithemtic) {
	int arraySize = 0;
	string buffer, result;
	double medianArray[2];
	for (int i = 0; i < expression.length(); i++) if (expression[i] == ',' || expression[i] == ']') arraySize++;
	double* numargsArray = new double[arraySize];
	string* argsArray = new string[arraySize];

	for (int i = 1; i < expression.length(); i++) {
		if ((expression[i] == ',' || expression[i] == ']') && buffer != "") {
			*argsArray = buffer;
			buffer = "";
			*numargsArray = EvalNormalEq(*argsArray);
			argsArray++;
			numargsArray++;
		}
		else buffer += expression[i];
	}

	numargsArray -= arraySize;
	argsArray -= arraySize;
	delete[] argsArray;

	if (IsArithemtic) {
		double sum = 0, differential;
		double diffCalcArray[2];
		int diffCalcArrayIndex = 0;
		bool diffCalculated = false;
		for (int i = 0; i < arraySize; i++) {
			sum += *numargsArray;
			if (diffCalcArrayIndex < 2) {
				diffCalcArray[diffCalcArrayIndex] = *numargsArray;
				diffCalcArrayIndex++;
			}
			numargsArray++;
		}
		differential = diffCalcArray[1] - diffCalcArray[0];
		numargsArray -= arraySize;
		double average = sum / arraySize;
		float medianIndex = (float)arraySize / 2;
		numargsArray += (int)floor(medianIndex);
		medianArray[0] = *numargsArray;
		numargsArray -= (int)floor(medianIndex);
		numargsArray += (int)ceil(medianIndex);
		medianArray[1] = *numargsArray;
		numargsArray -= (int)ceil(medianIndex);

		result = "This array has a:\nMedian:" + to_string((medianArray[0] + medianArray[1]) / 2) + "\nAverage:" + to_string(average) + "\nEach member is different by:" + to_string(differential);
	}
	else {
		double product = 1, average, diffArray[2];
		int diffArrayIndex = 0;
		for (int i = 0; i < arraySize; i++) {
			product += *numargsArray;
			if (diffArrayIndex < 2) {
				diffArray[diffArrayIndex] = *numargsArray;
				diffArrayIndex++;
			}
			numargsArray++;
		}
		numargsArray -= arraySize;
		average = product / arraySize;
		double differential = diffArray[1] / diffArray[0];
		
		result = "This array has a:\nAverage:" + to_string(average) + "\nDifferential:" + to_string(differential);
	}
	delete[] numargsArray;
	return result;
}

int returnPowerOfExpression(string input) {
	int varIndex;
	for (int i = 0; i < input.length(); i++) {
		if (isalpha(input[i])) {
			varIndex = i;
			break;
		}
	}
	if (varIndex == input.length() - 1) return 1;
	else return stoi(input.substr(varIndex + 2, input.length() - varIndex - 2));
}

string EvalSquare_EQ(string expression) {
	expression += '|';
	string arguments[4], buffer = "", varSign = "";
	int powerIndices[4] = { 2,1,1,1 };
	int argIndex = 0, currentPower = 0, tempIndex;
	double numArgs[4];
	bool writeFlag = false;
	for (int i = 0; i < expression.length(); i++) {
		if (argIndex > 1) writeFlag = true;
		if (isalpha(expression[i])) {
			writeFlag = true;
			buffer += expression[i];
		}
		else if ((expression[i] == '+' || expression[i] == '-' || expression[i] == '=' || expression[i] == '|') && writeFlag) {
			arguments[argIndex] = buffer;
			buffer = "";
			argIndex++;
			writeFlag = false;
		}
		else buffer += expression[i];
	}
	buffer = "";
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < arguments[i].length(); j++) {
			if (isalpha(arguments[i][j])) {
				arguments[i] = buffer;
				buffer = "";
				break;
			}
			else buffer += arguments[i][j];
		}
	}

	for (int i = 0; i < 4; i++) numArgs[i] = EvalNormalEq(arguments[i]);

	if (numArgs[4] > 0) {
		numArgs[3] -= numArgs[4];
		numArgs[4] = 0;
	}

	double result1, result2;

	result1 = -numArgs[2] + sqrt(pow(numArgs[2], 2) - 4 * numArgs[0] * numArgs[3]) / 2 * numArgs[0];
	result2 = -numArgs[2] - sqrt(pow(numArgs[2], 2) - 4 * numArgs[0] * numArgs[3]) / 2 * numArgs[0];

	string resultString = "The results are:\nx1:" + to_string(result1) + "\nx2:" + to_string(result2);

	return resultString;
}

string EvalEllipse(string expression) {
	string result = "Ellipse result placeholder";

	return result;
}

int main() {

	string expression, result;
	int optionIndex = EXPRCORRSIZE + 5, dotIndex = 0;

	while (optionIndex > 3) {
		cout << "Input the index of the expression type\n0. Algebra\n1. Square equation\n2. Arithmetic Series\n3. Geometric Series: ";
		cin >> optionIndex;
	}

	cout << endl << "Input the expression: ";
	cin >> ws;
	getline(cin, expression);
	expression = deSpaceify(expression);
	
	switch (optionIndex) {
	case 0:
		result = to_string(EvalNormalEq(expression));
		break;
	case 1:
		result = EvalSquare_EQ(expression);
		break;
	case 2: //arithmetic array
		result = EvalSeries(expression, true);
		break;
	case 3: //geometricarray
		result = EvalSeries(expression, false);
		break;
	default:
		result = "Invalid choice Index!";
		break;
	}
	
	cout << result << endl;
	
}

//TO-DO:
//	-analytical geometry
//	-truth statements
//	-serie's
//implement sin, cos, tg, ctg, pi


//reverse polish notaton