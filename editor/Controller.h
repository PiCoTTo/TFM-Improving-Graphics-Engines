#pragma once

template <class S, class O>
class Controller
{
public:
	Controller(S& subject, O& observer) : subject(subject), observer(observer) {}

private:
	S* subject;
	O* observer;
};