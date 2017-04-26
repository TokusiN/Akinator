#pragma once

#include <vector>

// ����A�C�e���ɑ΂��邠�鎿��̓���
class QuestionAnswer
{
public:
	QuestionAnswer() {};
	~QuestionAnswer() {};
	void Init(int y, int n);
	int yes;
	int no;
	double yes_possibility;
	double no_possibility;
};

// 20Q ��������Akinator�{��
class CTwentyAnswers
{
public:
	// ���J���\�b�h
	CTwentyAnswers();
	~CTwentyAnswers();
	void	init(int itemNum, int questionNum, std::vector<int> itemPlayCount, std::vector<std::vector<int> > qa);
	int		GetQuestion(int idx); // -1: It's ___?  -2: error
	int		GetAnswer(int idx); // 0:No 1:Yes
	double	GetCurrentEntoropy();
	double	GetMaxProb() { return m_maxProb; };
	int		GetMaxProbItem() { return m_maxProbItem; };
	bool	SendAnswer(int y); // 0:No 1:Yes
private:
	// �����o�ϐ�
	int		m_itemNum;
	int		m_questionNum;
	int		m_questionHistoryNum;
	int		m_entoropy;
	int		m_maxProbItem;
	double	m_maxProb;

	std::vector<int>							m_stockQuestion;
	std::vector<double>							m_itemPossibility;
	std::vector<std::vector<QuestionAnswer> >	m_qa;
	std::vector<int>							m_questionHistory;
	std::vector<int>							m_answerHistory;
	std::vector<int>							m_maxProbItemHistory;

	// ����J���\�b�h
	double CalcEntoropy(std::vector<double> possibility);
	void CalcMaxProb();
	bool MakeQuestion();

	std::vector<double> GetNextPossibirity(int question, int answer);
};
