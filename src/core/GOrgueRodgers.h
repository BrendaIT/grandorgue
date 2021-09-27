/*
* Copyright 2006 Milan Digital Audio LLC
* Copyright 2009-2019 GrandOrgue contributors (see AUTHORS)
* License GPL-2.0 or later (https://www.gnu.org/licenses/old-licenses/gpl-2.0.html).
*/

#ifndef GORGUERODERS_H
#define GORGUERODERS_H

#include <vector>
#include <stdint.h>

/* Bit test of the data field; returns clear, set or bit not in data */
enum class MIDI_BIT_STATE {
	MIDI_BIT_CLEAR,
	MIDI_BIT_SET,
	MIDI_BIT_NOT_PRESENT
};


uint8_t GORodgersChecksum(const std::vector<uint8_t>& msg, unsigned start, unsigned len);

MIDI_BIT_STATE GORodgersGetBit(unsigned stop, unsigned offset, const std::vector<uint8_t> data);
unsigned GORodgersSetBit(unsigned stop, bool state, std::vector<uint8_t>& data);

#endif
