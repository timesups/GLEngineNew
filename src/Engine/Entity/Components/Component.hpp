#pragma once
class Entity;




class Component
{
public:
	virtual ~Component() = default;

	virtual void Init() {}
	virtual void Update(float deltaTime) {}
	virtual void Render() {}

	Entity* GetEntity()const { return m_entity; }
protected:
	friend class Entity;
	void setEntity(Entity* entity) { m_entity = entity; }
private:
	Entity* m_entity = nullptr;
};