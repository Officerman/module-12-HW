#include <iostream>
#include <memory>
#include <string>

// Forward declaration of Context
class TicketMachine;

// Abstract State
class State {
public:
    virtual void selectTicket(TicketMachine& machine) { invalidOperation(); }
    virtual void insertMoney(TicketMachine& machine, double amount) { invalidOperation(); }
    virtual void dispenseTicket(TicketMachine& machine) { invalidOperation(); }
    virtual void cancelTransaction(TicketMachine& machine) { invalidOperation(); }
    virtual ~State() = default;

protected:
    void invalidOperation() const {
        std::cout << "Invalid operation in the current state.\n";
    }
};

// Context
class TicketMachine {
private:
    std::shared_ptr<State> currentState;
    double balance = 0.0;
    double ticketPrice = 10.0;

public:
    void setState(std::shared_ptr<State> state) {
        currentState = state;
    }

    void selectTicket() {
        currentState->selectTicket(*this);
    }

    void insertMoney(double amount) {
        currentState->insertMoney(*this, amount);
    }

    void dispenseTicket() {
        currentState->dispenseTicket(*this);
    }

    void cancelTransaction() {
        currentState->cancelTransaction(*this);
    }

    void setBalance(double amount) {
        balance = amount;
    }

    double getBalance() const {
        return balance;
    }

    double getTicketPrice() const {
        return ticketPrice;
    }
};

// Concrete States
class IdleState : public State {
public:
    void selectTicket(TicketMachine& machine) override {
        std::cout << "Ticket selected. Waiting for money.\n";
        machine.setState(std::make_shared<WaitingForMoneyState>());
    }
};

class WaitingForMoneyState : public State {
public:
    void insertMoney(TicketMachine& machine, double amount) override {
        double newBalance = machine.getBalance() + amount;
        machine.setBalance(newBalance);
        std::cout << "Inserted $" << amount << ". Current balance: $" << newBalance << "\n";

        if (newBalance >= machine.getTicketPrice()) {
            std::cout << "Sufficient money received.\n";
            machine.setState(std::make_shared<MoneyReceivedState>());
        }
    }

    void cancelTransaction(TicketMachine& machine) override {
        std::cout << "Transaction canceled. Returning to Idle state.\n";
        machine.setBalance(0);
        machine.setState(std::make_shared<IdleState>());
    }
};

class MoneyReceivedState : public State {
public:
    void dispenseTicket(TicketMachine& machine) override {
        std::cout << "Dispensing ticket...\n";
        machine.setBalance(0); // Reset balance
        machine.setState(std::make_shared<TicketDispensedState>());
    }

    void cancelTransaction(TicketMachine& machine) override {
        std::cout << "Transaction canceled. Returning money and resetting to Idle state.\n";
        machine.setBalance(0);
        machine.setState(std::make_shared<IdleState>());
    }
};

class TicketDispensedState : public State {
public:
    void selectTicket(TicketMachine& machine) override {
        std::cout << "Ticket already dispensed. Returning to Idle state.\n";
        machine.setState(std::make_shared<IdleState>());
    }
};

class TransactionCanceledState : public State {
public:
    void selectTicket(TicketMachine& machine) override {
        std::cout << "Transaction canceled. Returning to Idle state.\n";
        machine.setState(std::make_shared<IdleState>());
    }
};

// Main Function
int main() {
    TicketMachine machine;

    machine.setState(std::make_shared<IdleState>());

    machine.selectTicket();        // Move to WaitingForMoney
    machine.insertMoney(5.0);      // Add money but not enough
    machine.insertMoney(5.0);      // Add more money, move to MoneyReceived
    machine.dispenseTicket();      // Dispense ticket, move to TicketDispensed
    machine.selectTicket();        // Reset to Idle

    return 0;
}
