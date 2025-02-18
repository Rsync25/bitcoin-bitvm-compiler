// Gates.cpp
// Implemnetaions of the getas logic


#include "Gate.h"
#include "Circuit.h"

// compute the output of the NAND gate
void Gate_NAND::compute(void) {
	// compute the output of the NAND gate
	output[0]->set_value( !(input[0]->get_value() & input[1]->get_value()));
	is_computed = true;
}
// add the gate into the circuir
std::array<Connection*, 1> Gate_NAND::add_to_circuit(Circuit& circuit, std::array<Connection*, 2>& _input)  {
	circuit.add_gate(this);
	set_inputs(_input);
	return this->output;
}

// add a NOT gate into the circuir
std::array<Connection*,1> Gate_NOT::add_to_circuit(Circuit& circuit, std::array<Connection*, 1>& _inputs)  {
	// implemented with a nand gate
	Gate_NAND* _nand = new Gate_NAND();
	std::array<Connection*, 2> inputs2 = { _inputs[0], _inputs[0] };
	return _nand->add_to_circuit(circuit, inputs2);
};

// compute the output of the AND gate
std::array<Connection*, 1> Gate_AND::add_to_circuit(Circuit& circuit, std::array<Connection*, 2>& _inputs) {
	// implemented with a nand + not
	Gate_NAND* _nand	 = new Gate_NAND();
	Gate_NAND* _nand_not = new Gate_NAND();
	std::array<Connection*, 1> output_gate1 =  _nand->add_to_circuit(circuit, _inputs);
	std::array<Connection*, 2> output_gate2 = { output_gate1[0], output_gate1[0] };
	return _nand_not->add_to_circuit(circuit, output_gate2);
};

// compute the output of the OR gate
std::array<Connection*, 1> Gate_OR::add_to_circuit(Circuit& circuit, std::array<Connection*, 2>& _inputs)
{
	// implemented with r = not(a) nand not(b)
	Gate_NAND* _nand_a = new Gate_NAND();
	Gate_NAND* _nand_b = new Gate_NAND();
	Gate_NAND* _nand_r = new Gate_NAND();
	std::array<Connection*, 2> inputs_a = { _inputs[0], _inputs[0] };
	std::array<Connection*, 2> inputs_b = { _inputs[1], _inputs[1] };
	std::array<Connection*, 1> not_a = _nand_a->add_to_circuit(circuit, inputs_a);
	std::array<Connection*, 1> not_b = _nand_b->add_to_circuit(circuit, inputs_b);
	std::array<Connection*, 2> not_ab = { not_a[0], not_b[0] };
	return _nand_r->add_to_circuit(circuit, not_ab);

}

// compute the output of the XOR gate
std::array<Connection*, 1> Gate_XOR::add_to_circuit(Circuit& circuit, std::array<Connection*, 2>& _inputs)
{
	// implemented with 4 nand gates
	Gate_NAND* _nand_1 = new Gate_NAND();
	Gate_NAND* _nand_2 = new Gate_NAND();
	Gate_NAND* _nand_3 = new Gate_NAND();
	Gate_NAND* _nand_4 = new Gate_NAND();
	// nand(a, b)
	std::array<Connection*, 2> inputs_1 = { _inputs[0], _inputs[1] };
	std::array<Connection*, 1> nand_ab = _nand_1->add_to_circuit(circuit, inputs_1);
	// nand(a, nand(a, b))
	std::array<Connection*, 2> inputs_2 = { _inputs[0], nand_ab[0] };
	std::array<Connection*, 1> nand_a_nand_ab = _nand_2->add_to_circuit(circuit, inputs_2);
	// nand(b, nand(a, b))
	std::array<Connection*, 2> inputs_3 = { _inputs[1], nand_ab[0] };
	std::array<Connection*, 1> nand_b_nand_ab = _nand_3->add_to_circuit(circuit, inputs_3);
	// result
	std::array<Connection*, 2> inputs_4 = { nand_a_nand_ab[0], nand_b_nand_ab[0] };
	std::array<Connection*, 1> xor_ab = _nand_4->add_to_circuit(circuit, inputs_4);

	return  xor_ab;

}

