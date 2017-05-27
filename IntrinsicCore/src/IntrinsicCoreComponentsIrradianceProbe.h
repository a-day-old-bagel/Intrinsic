// Copyright 2016 Benjamin Glatzel
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

namespace Intrinsic
{
namespace Core
{
namespace Components
{
typedef Dod::Ref IrradianceProbeRef;
typedef _INTR_ARRAY(IrradianceProbeRef) IrradianceProbeRefArray;

struct IrradianceProbeData : Dod::Components::ComponentDataBase
{
  IrradianceProbeData()
      : Dod::Components::ComponentDataBase(
            _INTR_MAX_IRRADIANCE_PROBE_COMPONENT_COUNT)
  {
    descRadius.resize(_INTR_MAX_IRRADIANCE_PROBE_COMPONENT_COUNT);
    descPriority.resize(_INTR_MAX_IRRADIANCE_PROBE_COMPONENT_COUNT);

    descSHDay.resize(_INTR_MAX_IRRADIANCE_PROBE_COMPONENT_COUNT);
    descSHNight.resize(_INTR_MAX_IRRADIANCE_PROBE_COMPONENT_COUNT);
  }

  _INTR_ARRAY(float) descRadius;
  _INTR_ARRAY(uint32_t) descPriority;

  _INTR_ARRAY(SHCoeffs) descSHDay;
  _INTR_ARRAY(SHCoeffs) descSHNight;
};

struct IrradianceProbeManager
    : Dod::Components::ComponentManagerBase<
          IrradianceProbeData, _INTR_MAX_IRRADIANCE_PROBE_COMPONENT_COUNT>
{
  static void init();

  // <-

  _INTR_INLINE static IrradianceProbeRef
  createIrradianceProbe(Entity::EntityRef p_ParentEntity)
  {
    IrradianceProbeRef ref = Dod::Components::ComponentManagerBase<
        IrradianceProbeData, _INTR_MAX_IRRADIANCE_PROBE_COMPONENT_COUNT>::
        _createComponent(p_ParentEntity);
    return ref;
  }

  // <-

  _INTR_INLINE static void resetToDefault(MeshRef p_Ref)
  {
    _descRadius(p_Ref) = 20.0f;
    memset(&_descSHDay(p_Ref), 0x00, sizeof(SHCoeffs));
    memset(&_descSHNight(p_Ref), 0x00, sizeof(SHCoeffs));
  }

  // <-

  _INTR_INLINE static void
  destroyIrradianceProbe(IrradianceProbeRef p_IrradianceProbe)
  {
    Dod::Components::ComponentManagerBase<
        IrradianceProbeData, _INTR_MAX_IRRADIANCE_PROBE_COMPONENT_COUNT>::
        _destroyComponent(p_IrradianceProbe);
  }

  // <-

  _INTR_INLINE static void compileDescriptor(IrradianceProbeRef p_Ref,
                                             bool p_GenerateDesc,
                                             rapidjson::Value& p_Properties,
                                             rapidjson::Document& p_Document)
  {
    p_Properties.AddMember("radius",
                           _INTR_CREATE_PROP(p_Document, p_GenerateDesc,
                                             _N(IrradianceProbe), _N(float),
                                             _descRadius(p_Ref), false, false),
                           p_Document.GetAllocator());
    p_Properties.AddMember(
        "priority",
        _INTR_CREATE_PROP(p_Document, p_GenerateDesc, _N(IrradianceProbe),
                          _N(float), _descPriority(p_Ref), false, false),
        p_Document.GetAllocator());

    p_Properties.AddMember("shDay",
                           _INTR_CREATE_PROP(p_Document, p_GenerateDesc,
                                             _N(IrradianceProbe), _N(sh),
                                             _descSHDay(p_Ref), false, false),
                           p_Document.GetAllocator());
    p_Properties.AddMember("shNight",
                           _INTR_CREATE_PROP(p_Document, p_GenerateDesc,
                                             _N(IrradianceProbe), _N(sh),
                                             _descSHNight(p_Ref), false, false),
                           p_Document.GetAllocator());
  }

  // <-

  _INTR_INLINE static void initFromDescriptor(IrradianceProbeRef p_Ref,
                                              rapidjson::Value& p_Properties)
  {
    if (p_Properties.HasMember("radius"))
      _descRadius(p_Ref) =
          JsonHelper::readPropertyFloat(p_Properties["radius"]);
    if (p_Properties.HasMember("priority"))
      _descPriority(p_Ref) =
          (uint32_t)JsonHelper::readPropertyFloat(p_Properties["priority"]);

    if (p_Properties.HasMember("shDay"))
      _descSHDay(p_Ref) = JsonHelper::readPropertySH(p_Properties["shDay"]);
    if (p_Properties.HasMember("shNight"))
      _descSHNight(p_Ref) = JsonHelper::readPropertySH(p_Properties["shNight"]);
  }

  // <-

  _INTR_INLINE static void sortByPriority(IrradianceProbeRefArray& p_Probes)
  {
    _INTR_PROFILE_CPU("General", "Sort Irradiance Probes");

    struct Comparator
    {
      bool operator()(const Dod::Ref& a, const Dod::Ref& b) const
      {
        return _descPriority(a) < _descPriority(b);
      }
    } comp;

    Algorithm::parallelSort<Dod::Ref, Comparator>(p_Probes, comp);
  }

  // <-

  // Getter/Setter
  // ->

  // Description
  _INTR_INLINE static float& _descRadius(IrradianceProbeRef p_Ref)
  {
    return _data.descRadius[p_Ref._id];
  }
  _INTR_INLINE static uint32_t& _descPriority(IrradianceProbeRef p_Ref)
  {
    return _data.descPriority[p_Ref._id];
  }

  _INTR_INLINE static SHCoeffs& _descSHDay(IrradianceProbeRef p_Ref)
  {
    return _data.descSHDay[p_Ref._id];
  }
  _INTR_INLINE static SHCoeffs& _descSHNight(IrradianceProbeRef p_Ref)
  {
    return _data.descSHNight[p_Ref._id];
  }
};
}
}
}