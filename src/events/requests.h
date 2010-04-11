/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file events/requests.h
 *  Inter-thread request events.
 */

#include "common/types.h"
#include "common/mutex.h"

#include "events/types.h"

#ifndef EVENTS_REQUESTS_H
#define EVENTS_REQUESTS_H

namespace Graphics {
	class ImageDecoder;
}

namespace Events {

/** Base class for request events.
 *
 *  Requests are the main scheme of communication between the game thread and
 *  the main thread, which handles all low-level event and graphics operations.
 *
 *  The idea behind requests is that, as soon as the game thread knows exactly
 *  what it needs, it creates and dispatches a specific request. Then it does
 *  other things, until it needs what it requested for, issuing a waitReply on
 *  the request. That way, the actual fulfilling of the request can happen
 *  asynchronously, without it unnecessarily blocking further execution of the
 *  game thread.
 */
class Request {
// To be used by the game thread
public:
	Request();

	/** Dispatch the request. */
	void dispatch();

	/** Wait until the request has been answered. */
	void waitReply();

protected:
	/** Create the event for this request. */
	void createEvent(ITCEvent itcType);

private:
	Common::Mutex _mutexUse;   ///< The mutex preventing race condition in the use.
	Common::Mutex _mutexReply; ///< The mutex signaling a reply.

	Event _event; ///< The actual event.

	volatile bool _dispatched; ///< Was the event dispatched?


// To be used by the event thread
public:
	/** Signal that the request has been answered. */
	void signalReply();
};

/** Requesting fullscreen/windowed mode. */
class RequestFullscreen : public Request {
public:
	RequestFullscreen(bool fullscreen);
};

/** Requesting display size change. */
class RequestResize : public Request {
public:
	RequestResize(int width, int height);

private:
	int _width;
	int _height;


// To be used by the event thread
public:
	int getWidth()  const;
	int getHeight() const;
};

/** Request the creation of textures. */
class RequestCreateTextures : public Request {
public:
	RequestCreateTextures(uint32 n);
	~RequestCreateTextures();

	const uint32 *getIDs() const;

private:
	uint32 _count;
	uint32 *_ids;


// To be used by the event thread
public:
	uint32 getCount() const;

	uint32 *getIDs();
};

/** Request the destruction of textures. */
class RequestDestroyTextures : public Request {
public:
	RequestDestroyTextures(uint32 n, uint32 *ids);
	~RequestDestroyTextures();

private:
	uint32 _count;
	uint32 *_ids;


// To be used by the event thread
public:
	uint32 getCount() const;

	const uint32 *getIDs() const;
};

/** Request the loading of texture image data. */
class RequestLoadTextures : public Request {
public:
	RequestLoadTextures(uint32 id, const byte *data, int width, int height, bool hasAlpha = false);
	RequestLoadTextures(uint32 id, const Graphics::ImageDecoder *image);

private:
	uint32 _id;
	const byte *_data;
	int _width, _height;
	bool _hasAlpha;


// To be used by the event thread
public:
	uint32 getID() const;
	const byte *getData() const;
	int getWidth() const;
	int getHeight() const;
	bool hasAlpha() const;
};

} // End of namespace Events

#endif // EVENTS_REQUESTS_H
