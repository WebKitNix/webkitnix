/*
 * Copyright (C) 2001, 2002 Apple Computer, Inc.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#include <qobject.h>
#include <qvariant.h>
#include <qguardedptr.h>

#include <kwqdebug.h>

bool QObject::connect(const QObject *sender, const char *signal, const QObject *dest, 
    const char *slot)
{
    if (sender)
        ((QObject *)sender)->setTarget ((QObject *)dest);
    KWQDEBUG ("src = %p, signal = %s, dest = %p, slot = %s\n", sender, signal, dest, slot);
    return FALSE;
}



bool QObject::connect(const QObject *sender, const char *signal, const char *slot) const
{
    if (sender)
        ((QObject *)sender)->setTarget ((QObject *)sender);
    KWQDEBUG ("src = %p, signal = %s, slot = %s\n", sender, signal, slot);
    return FALSE;    
}


void QObject::emitAction(QObject::Actions action)
{
    if (target != nil)
        target->performAction (action);
}


void QObject::performAction(QObject::Actions action)
{
    KWQDEBUG ("action = %d\n", action);
}


bool QObject::disconnect( const QObject *, const char *, const QObject *, 
    const char *)
{
    return FALSE;
}

void QObject::setTarget (QObject *t)
{
    target = t;
}



QObject::QObject(QObject *parent=0, const char *name=0)
{
    guardedPtrDummyList.append(this);
}


QObject::~QObject()
{
}


// member functions --------------------------------------------------------

const char *QObject::name() const
{
    _logNotYetImplemented();
    return "noname";
}


void QObject::setName(const char *)
{
    _logNotYetImplemented();
}

QVariant QObject::property(const char *name) const
{
    _logNeverImplemented();
    return QVariant();
}

bool QObject::inherits(const char *) const
{
    _logNeverImplemented();
    return FALSE;
}

@interface KWQTimerCallback : NSObject
{
    QObject *target;
    int timerId;
}
- initWithQObject: (QObject *)object timerId: (int)timerId;
- (void)timerFired: (id)context;
@end

@implementation KWQTimerCallback
- initWithQObject: (QObject *)qo timerId: (int)t
{
    [super init];
    timerId = t;
    target = qo;
    return self;
}

- (void)timerFired: (id)context
{
    QTimerEvent te(timerId);
    target->timerEvent (&te);
}
@end

int timerCount = 1;

NSMutableDictionary *timers;

void QObject::timerEvent (QTimerEvent *te)
{
}

int QObject:: startTimer(int milliseconds)
{
    NSNumber *timerId = [NSNumber numberWithInt: timerCount];
    
    if (timers == nil){
        // The timers dictionary itself leaks, but the contents are removed
        // when a timer expires or is killed.
        timers = [[NSMutableDictionary alloc] init];
    }
    NSTimer *timer = [NSTimer scheduledTimerWithTimeInterval: ((NSTimeInterval)milliseconds)/1000
                target: [[[KWQTimerCallback alloc] initWithQObject: this timerId: timerCount] autorelease]
                selector: @selector(timerFired:)
                userInfo: timerId
                repeats: NO];
    [timers setObject: timer forKey: timerId];
        
    return timerCount++;    
}


void QObject::killTimer(int _timerId)
{
    NSNumber *timerId = [NSNumber numberWithInt: _timerId];
    NSTimer *timer;
    
    timer = (NSTimer *)[timers objectForKey: timerId];
    [timer invalidate];
    [timers removeObjectForKey: timerId];
}


void QObject::killTimers()
{
    NSArray *contexts;
    NSNumber *key;
    NSTimer *timer;
    int i, count;
    
    contexts = [timers allKeys];
    count = [contexts count];
    for (i = 0; i < count; i++){
        key = (NSNumber *)[contexts objectAtIndex: i];
        timer = (NSTimer *)[timers objectForKey: key];
        [timer invalidate];
        [timers removeObjectForKey: key];
    }
}


void QObject::installEventFilter(const QObject *)
{
    _logNeverImplemented();
}


void QObject::removeEventFilter(const QObject *)
{
    _logNeverImplemented();
}

bool QObject::eventFilter(QObject *o, QEvent *e)
{
    _logNeverImplemented();
    return false;
}


void QObject::blockSignals(bool)
{
    _logNeverImplemented();
}

