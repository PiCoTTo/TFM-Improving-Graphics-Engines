#pragma once

class RenderPass
{
public:
	virtual ~RenderPass() {}

	virtual void update(float deltaTime) {}
	virtual void render() {}
};