//
// File:       nbody.cl
//
// Abstract:   This example performs an NBody simulation which calculates a gravity field 
//             and corresponding velocity and acceleration contributions accumulated 
//             by each body in the system from every other body.  This example
//             also shows how to mitigate computation between all available devices
//             including CPU and GPU devices, as well as a hybrid combination of both,
//             using separate threads for each simulator.
//
// Version:    <1.0>
//
// Disclaimer: IMPORTANT:  This Apple software is supplied to you by Apple Inc. ("Apple")
//             in consideration of your agreement to the following terms, and your use,
//             installation, modification or redistribution of this Apple software
//             constitutes acceptance of these terms.  If you do not agree with these
//             terms, please do not use, install, modify or redistribute this Apple
//             software.
//
//             In consideration of your agreement to abide by the following terms, and
//             subject to these terms, Apple grants you a personal, non - exclusive
//             license, under Apple's copyrights in this original Apple software ( the
//             "Apple Software" ), to use, reproduce, modify and redistribute the Apple
//             Software, with or without modifications, in source and / or binary forms;
//             provided that if you redistribute the Apple Software in its entirety and
//             without modifications, you must retain this notice and the following text
//             and disclaimers in all such redistributions of the Apple Software. Neither
//             the name, trademarks, service marks or logos of Apple Inc. may be used to
//             endorse or promote products derived from the Apple Software without specific
//             prior written permission from Apple.  Except as expressly stated in this
//             notice, no other rights or licenses, express or implied, are granted by
//             Apple herein, including but not limited to any patent rights that may be
//             infringed by your derivative works or by other works in which the Apple
//             Software may be incorporated.
//
//             The Apple Software is provided by Apple on an "AS IS" basis.  APPLE MAKES NO
//             WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE IMPLIED
//             WARRANTIES OF NON - INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A
//             PARTICULAR PURPOSE, REGARDING THE APPLE SOFTWARE OR ITS USE AND OPERATION
//             ALONE OR IN COMBINATION WITH YOUR PRODUCTS.
//
//             IN NO EVENT SHALL APPLE BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL OR
//             CONSEQUENTIAL DAMAGES ( INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
//             SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
//             INTERRUPTION ) ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION, MODIFICATION
//             AND / OR DISTRIBUTION OF THE APPLE SOFTWARE, HOWEVER CAUSED AND WHETHER
//             UNDER THEORY OF CONTRACT, TORT ( INCLUDING NEGLIGENCE ), STRICT LIABILITY OR
//             OTHERWISE, EVEN IF APPLE HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Copyright ( C ) 2008 Apple Inc. All Rights Reserved.
//

////////////////////////////////////////////////////////////////////////////////

#define LOOP_UNROLL 1

float4
ComputeForce(float4 bi, float4 bj, float softeningSquared)
{
    float4 r;

    r = bi - bj;
    float distSqr = r.x * r.x + r.y * r.y + r.z * r.z;
    distSqr += softeningSquared;

    float invDist = 1.0f / sqrtf(distSqr);
    float invDistCube =  invDist * invDist * invDist;

    float s = bj.w * invDistCube;

    return r * s;
}

__kernel void
IntegrateSystem(__global float4 *newPos, __global float4 *newVel,
                     __global float4 *oldPos, __global float4 *oldVel,
                     __local float4 *sharedPos, const float deltaTime,
                     const float damping, const float softening, const int body_count)
{
    int index 		= get_global_thread_id(0);
    int tidx 		= get_local_thread_id(0);
    int blockDimX 	= get_local_thread_size(0);

    int tile = 0;

    float4 pos = oldPos[index];
    float mass = pos.w;
    float softeningSq = softening * softening;

    float4 acc = (float4)(0.0f, 0.0f, 0.0f, 0.0f);
    int i, j;

    for (i = 0; i < body_count; i += blockDimX, tile++)
    {
        sharedPos[tidx] = oldPos[tile * blockDimX + tidx];

        barrier(CL_LOCAL_MEM_FENCE);

        for (j = 0; j < blockDimX; )
        {
            acc += ComputeForce(sharedPos[j++], pos, softeningSq);
#if LOOP_UNROLL >= 1
            acc += ComputeForce(sharedPos[j++], pos, softeningSq);
#endif
#if LOOP_UNROLL >= 2
            acc += ComputeForce(sharedPos[j++], pos, softeningSq);
            acc += ComputeForce(sharedPos[j++], pos, softeningSq);
#endif
#if LOOP_UNROLL >= 4
            acc += ComputeForce(sharedPos[j++], pos, softeningSq);
            acc += ComputeForce(sharedPos[j++], pos, softeningSq);
            acc += ComputeForce(sharedPos[j++], pos, softeningSq);
            acc += ComputeForce(sharedPos[j++], pos, softeningSq);
#endif
        }

        barrier(CL_LOCAL_MEM_FENCE);

    }

    float4 vel = oldVel[index];

    vel += acc * deltaTime;
    vel *= damping;

    pos += vel * deltaTime;
    pos.w = mass;

    newPos[index] = pos;
    newVel[index] = vel;
    newPos[index] = (float4)(index, tidx, blockDimX, 0.0f);

}
