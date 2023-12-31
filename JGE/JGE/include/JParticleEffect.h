//-------------------------------------------------------------------------------------
//
// JGE++ is a hardware accelerated 2D game SDK for PSP/Windows.
//
// Licensed under the BSD license, see LICENSE in JGE root for details.
// 
// Copyright (c) 2007 James Hui (a.k.a. Dr.Watson) <jhkhui@gmail.com>
// 
//-------------------------------------------------------------------------------------

#ifndef __PARTICLE_EFFECT_H__
#define __PARTICLE_EFFECT_H__

#define MAX_EMITTER		5

class JParticleSystem;
class JParticleEmitter;
class JResourceManager;


//////////////////////////////////////////////////////////////////////////
/// Particle effect. Each particle effect can contain one or more emitters.
/// 
//////////////////////////////////////////////////////////////////////////
class JParticleEffect
{
public:

	//////////////////////////////////////////////////////////////////////////
	/// Constructor.
	/// 
	/// @param mgr - Resource manager for retrieving image quads for the particles.
	/// 
	//////////////////////////////////////////////////////////////////////////
 	JParticleEffect(JResourceManager* mgr);

	~JParticleEffect();

	//////////////////////////////////////////////////////////////////////////
	/// Load effect from file.
	/// 
	/// @param filename - Name of effect file.
	/// 
	//////////////////////////////////////////////////////////////////////////
	bool Load(const char* filename);
	
	
	//////////////////////////////////////////////////////////////////////////
	/// Update particle effect.
	/// 
	/// @param dt - Time elapsed since last update (in second).
	/// 
	//////////////////////////////////////////////////////////////////////////
	void Update(float dt);

	//////////////////////////////////////////////////////////////////////////
	/// Render particle effect.
	/// 
	//////////////////////////////////////////////////////////////////////////
	void Render();
	
	//////////////////////////////////////////////////////////////////////////
	/// Check if the particle effect is finished.
	/// 
	/// @return True if done.
	/// 
	//////////////////////////////////////////////////////////////////////////
	bool Done();

	//////////////////////////////////////////////////////////////////////////
	/// Start playing.
	/// 
	//////////////////////////////////////////////////////////////////////////
	void Start();

	//////////////////////////////////////////////////////////////////////////
	/// Stop playing.
	/// 
	//////////////////////////////////////////////////////////////////////////
	void Stop();

	//////////////////////////////////////////////////////////////////////////
	/// Set particle system.
	/// 
	/// @param particleSys - Particle system.
	/// 
	//////////////////////////////////////////////////////////////////////////
	void SetParticleSystem(JParticleSystem* particleSys);

	//////////////////////////////////////////////////////////////////////////
	/// Get particle system.
	/// 
	/// @return Particle system.
	/// 
	//////////////////////////////////////////////////////////////////////////
	JParticleSystem* GetParticleSystem();

	//////////////////////////////////////////////////////////////////////////
	/// Set position of the effect. New particles will be emitted from the
	/// new position but the existing active particles will not be affected.
	/// 
	/// @param x - X screen position.
	/// @param y - Y screen position.
	/// 
	//////////////////////////////////////////////////////////////////////////
	void SetPosition(float x, float y);

	//////////////////////////////////////////////////////////////////////////
	/// Get X position.
	/// 
	/// @return X position.
	///
	//////////////////////////////////////////////////////////////////////////
	float GetX();

	//////////////////////////////////////////////////////////////////////////
	/// Get Y position.
	/// 
	/// @return Y position.
	///
	//////////////////////////////////////////////////////////////////////////
	float GetY();

	//////////////////////////////////////////////////////////////////////////
	/// Move the particle effect over to a new position. All the existing 
	/// particles will be moved relatively.
	/// 
	/// @param X - X screen position.
	/// @param y - Y screen position.
	/// 
	//////////////////////////////////////////////////////////////////////////
	void MoveTo(float x, float y);
	
protected:
	JParticleSystem* mParticleSystem;
	JResourceManager* mResourceManager;
	float mX;
	float mY;

	int mEmitterCount;
	JParticleEmitter* mParticleEmitters[MAX_EMITTER];
};


#endif

