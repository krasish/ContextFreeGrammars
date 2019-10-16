#pragma once

class Identidied {
protected:
	unsigned id;
public:
	Identidied() : id(0) {};
	Identidied(unsigned _id) : id(_id) {};
	Identidied(const Identidied& other) : id(other.id) {}
	unsigned get_id() const { return id; }
	void set_id(unsigned id) { this->id = id; }
};