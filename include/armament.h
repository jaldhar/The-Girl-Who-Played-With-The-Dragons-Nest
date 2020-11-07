#ifndef ARMAMENT_H
#define ARMAMENT_H

#include <memory>

class Armament
{
public:
    Armament();
    Armament(int defenseBonus, int offenseBonus);
    virtual ~Armament();
    int  defenseBonus() const;
    void setDefenseBonus(int defenseBonus);
    int  offenseBonus() const;
    void setOffenseBonus(int offenseBonus);

private:
    struct ArmamentImpl;
    std::unique_ptr<ArmamentImpl> impl_;
};

#endif // ARMAMENT_H
