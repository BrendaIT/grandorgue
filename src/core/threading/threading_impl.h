/*
* Copyright 2006 Milan Digital Audio LLC
* Copyright 2009-2021 GrandOrgue contributors (see AUTHORS)
* License GPL-2.0 or later (https://www.gnu.org/licenses/old-licenses/gpl-2.0.html).
*/

#ifndef THREADING_IMPL_H
#define THREADING_IMPL_H

#include <wx/log.h>

#define WAIT_TIMEOUT_MS 1000

#if defined GO_STD_MUTEX
#include <chrono>
extern std::chrono::milliseconds const THREADING_WAIT_TIMEOUT;
#endif

#endif /* COMMON_IMPL_H */

