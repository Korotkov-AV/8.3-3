// 8.3-3.cpp : 



#include <iostream>
#include <fstream>

enum Type {
    fatalError,
    error,
    warning,
    unknown
};

class LogMessage {
    Type t;
    std::string m;
public:
    LogMessage(Type type, std::string mes) : t(type), m(mes) {};
    LogMessage(std::string mes, Type type) : t(type), m(mes) {};
    Type type() const {
        return t;
    }
    const std::string& message() const {
        return m;
    }
};

void printConsole(const std::string& message) {
    std::cout << message << std::endl;
}

void printFile(const std::string& message) {
    std::ofstream file("text.txt");
    file << message << std::endl;
    file.close();
}

class Handler {
public:
    virtual Handler* SetNextHandler(Handler* h) = 0;
    virtual void Handle(LogMessage LM) = 0;
    virtual void Delete() = 0;
};

class AbstractHandler : public Handler {
    Handler* next_handler;
public:
    AbstractHandler() : next_handler(nullptr) {};
    Handler* SetNextHandler(Handler* h) override {
        this->next_handler = h;
        return h;
    }
    void Handle(LogMessage LM) override {
        if (this->next_handler) { //next_handler != nullptr
            this->next_handler->Handle(LM);
        }
        else std::cout << "Nothing is processsing";
    }
    void Delete() override {
        if (this->next_handler) {
            this->next_handler->Delete();
        }
        delete this;
    }
};

class FatalError : public AbstractHandler {
public:
    void Handle(LogMessage LM) override {
        if (LM.type() == Type::fatalError) {
            throw std::string("Fatal error message: " + LM.message());
        }
        else {
            AbstractHandler::Handle(LM);
        }
    }
};

class Error : public AbstractHandler {
public:
    void Handle(LogMessage LM) override {
        if (LM.type() == Type::error) {
            printFile("Error message: " + LM.message());
        }
        else {
            AbstractHandler::Handle(LM);
        }
    }
};

class Warning : public AbstractHandler {
public:
    void Handle(LogMessage LM) override {
        if (LM.type() == Type::warning) {
            printConsole("Warning message: " + LM.message());
        }
        else {
            AbstractHandler::Handle(LM);
        }
    }
};

class Unknown : public AbstractHandler {
public:
    void Handle(LogMessage LM) override {
        if (LM.type() == Type::unknown) {
            throw std::string("Unknown message: " + LM.message());
        }
        else {
            AbstractHandler::Handle(LM);
        }
    }
};

class Chain {
    FatalError* fe;
    Error* e;
    Warning* w;
    Unknown* u;
public:
    Chain() {
        fe = new FatalError;
        e = new Error;
        w = new Warning;
        u = new Unknown;
        fe->SetNextHandler(e)->SetNextHandler(w)->SetNextHandler(u);
    }
    void Send(LogMessage LM) {
        try {
            fe->Handle(LM);
        }
        catch (std::string& mes) {
            std::cout << "this is new exeption: " << std::endl;
            std::cout << mes << std::endl;
        }
    }
    ~Chain() {
        delete fe;
        delete e;
        delete w;
        delete u;
    }
};

int main() {
    Chain MyChain;
    LogMessage mes1("error1", Type::unknown);
    MyChain.Send(mes1);
}