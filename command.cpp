#include <iostream>
#include <string>
#include <vector>
#include <sstream>
using namespace std;

// Абстрактная команда задаёт общий интерфейс для конкретных
// классов команд и содержит базовое поведение отмены операции
class Command {
public:
	virtual ~Command() {}
	virtual void Execute() = 0;
	virtual void Undo() = 0;
};

// Класс редактора содержит непосредственные операции над
// текстом. Он отыгрывает роль получателя — команды делегируют
// ему свои действия
class Editor {
public:
	Editor() {}
	Editor(const string& filepath) : filepath(filepath) {}
	void Save() {
		cout << "Реализация сохранения файла\n";
	}
	void SaveAs(const string& newpath) {
		cout << "Реализация сохранения файла по новому пути\n";
	}
	void Open(const string& filepath) {
		cout << "Реализация открытия файла\n";
	}
	void Print() {
		cout << "Реализация печати файла\n";
	}
	void Close() {
		cout << "Реализация закрытия файла\n";
	}
	void Revert() {
		cout << "Реализация отката последнего изменения\n";
	}
	void CreateNew() {
		cout << "Реализация создания нового файла\n";
	}
	void CloneRepository(const string& repositoryurl) {
		cout << "Реализация клонирования репозитория\n";
	}
	void SetPath(const string& path) {
		filepath = path;
	}
	string GetPath() const {
		return filepath;
	}
private:
	string filepath;
	string filecontent;
};

// Конкретные команды
class SaveCommand : public Command {
public:
	SaveCommand(Editor* editor) : editor(editor) {}
	void Execute() override { editor->Save(); }
	void Undo() override { editor->Revert(); }
private:
	Editor* editor;
};

class SaveAsCommand : public Command {
public:
	SaveAsCommand(Editor* editor, const string& newpath) :
		editor(editor), newpath(newpath), oldpath(editor->GetPath()) {}
	void Execute() override { editor->SaveAs(newpath); }
	void Undo() override { editor->SetPath(oldpath); }
private:
	Editor* editor;
	string oldpath;
	string newpath;
};

class OpenCommand : public Command {
public:
	OpenCommand(Editor* editor, const string& filepath) :
		editor(editor), filepath(filepath) {}
	void Execute() override { editor->Open(filepath); }
	void Undo() override { editor->Close(); }
private:
	Editor* editor;
	string filepath;
};

class PrintCommand : public Command {
public:
	PrintCommand(Editor* editor) : editor(editor) {}
	void Execute() override { editor->Print(); }
	void Undo() override {}
private:
	Editor* editor;
};

class CloseCommand : public Command {
public:
	CloseCommand(Editor* editor) : editor(editor) {}
	void Execute() override { editor->Close(); }
	void Undo() override { editor->Open(editor->GetPath()); }
private:
	Editor* editor;
};

class NewCommand : public Command {
public:
	NewCommand(Editor* editor) : editor(editor) {}
	void Execute() override { editor->CreateNew(); }
	void Undo() override { editor->Close(); }
private:
	Editor* editor;
};

// Глобальная история команд — это стек
class CommandHistory {
public:
	void Push(Command* command) {
		commands.push_back(command);
	}
	Command* Pop() {
		Command* command = commands.back();
		commands.pop_back();
		return command;
	}
private:
	vector<Command*> commands;
};

// Класс приложения настраивает объекты для совместной работы.
// Он выступает в роли отправителя — создаёт команды, чтобы
// выполнить какие-то действия
class Application {
public:
	Application() {}
	void Run() {
		while (true) {
			// Получаем команду от пользователя
			cout << "Пожалуйста, введите команду. Например, open, save, saveas, close, print, new.\n";
			string input;
			getline(cin, input);

			// Создаем соответствующую команду
			Command* command = ParseInput(input);

			// Выполняем команду
			if (command != nullptr) {
				command->Execute();
				// Сохраняем выполненную команду в истории
				commandhistory.Push(command);
			}
			else {
				cout << "Программа не может распознать команду :(\n";
			}
		}
	}
private:
	CommandHistory commandhistory;

	Command* ParseInput(const string& input) {
		// Парсим команду и аргументы
		vector<string> tokens = TokenizeInput(input);
		string commandname = tokens[0];
		vector<string> args = tokens;
		args.erase(args.begin()); // Удаляем имя команды из аргументов

		// Создаем соответствующую команду
		if (commandname == "save") {
			return new SaveCommand(&editor);
		}
		else if (commandname == "saveas") {
			if (args.empty()) {
				cout << "Пропущен аргумент: newpath" << endl;
				return nullptr;
			}
			return new SaveAsCommand(&editor, args[0]);
		}
		else if (commandname == "open") {
			if (args.empty()) {
				cout << "Пропущен аргумент: filepath" << endl;
				return nullptr;
			}
			return new OpenCommand(&editor, args[0]);
		}
		else if (commandname == "print") {
			return new PrintCommand(&editor);
		}
		else if (commandname == "close") {
			return new CloseCommand(&editor);
		}
		else if (commandname == "new") {
			return new NewCommand(&editor);
		}
		else {
			return nullptr;
		}
	}

	vector<string> TokenizeInput(const string& input) {
		vector<string> tokens;
		istringstream iss(input);
		string token;
		while (iss >> token) {
			tokens.push_back(token);
		}
		return tokens;
	}

	Editor editor;
};

int main() {
	setlocale(0, "");
	Application app;
	app.Run();
}

/*
Данный код представляет пример паттерна "Команда" (Command). В этом
примере рассматривается текстовый редактор, где пользователь может
вводить команды (например, "save", "open", "print", "close" и т.д.),
а редактор должен выполнить соответствующие действия. Для каждой команды
создается свой класс, наследующийся от абстрактного класса Command,
и реализующий методы Execute() и Undo(). Также имеется класс Editor,
реализующий методы, выполняющие непосредственные операции над текстом
(открытие, сохранение, печать, закрытие и т.д.). Класс Application
является отправителем, который создает соответствующие команды, вызывает
их метод Execute() и добавляет в историю выполненных команд
(CommandHistory), чтобы в дальнейшем можно было их отменить (метод Undo()).

В общем, данный код демонстрирует, как можно разделить операции
на мелкие шаги, каждый из которых представлен в виде объекта-команды,
что обеспечивает гибкость и удобство управления операциями в приложении.
*/

// https://refactoring.guru/ru/design-patterns/command

/*
Команда — это поведенческий паттерн проектирования, который превращает
запросы в объекты, позволяя передавать их как аргументы при вызове
методов, ставить запросы в очередь, логировать их, а также поддерживать
отмену операций.
*/