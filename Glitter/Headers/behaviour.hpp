#ifndef BEHAVIOUR_HPP
#define BEHAVIOUR_HPP

class Cube;

class Behaviour {
    public:
        Behaviour(Cube & obj) : object(obj) {
            std::cout << "behaviour constructor" << "\n";
        }
        virtual void onStart() = 0;
        virtual void onUpdate() = 0;
    protected:
        Cube & object;
};

#endif /* BEHAVIOUR_HPP */
