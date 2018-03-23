within Buildings.ThermalZones.EnergyPlus.Validation;
model ThreeZonesTwoBuildings
  "Validation model for three zones that are in two buildings"
  extends Modelica.Icons.Example;
  package Medium = Buildings.Media.Air "Medium model";

  parameter String fmuName = "aaa.fmu" "Name of the FMU file that contains this zone";

  ThermalZone zon1(
    zoneName="Zone 1",
    redeclare package Medium = Medium,
    energyDynamics=Modelica.Fluid.Types.Dynamics.FixedInitial,
    fmuName="bld1.fmu")            "Thermal zone"
    annotation (Placement(transformation(extent={{20,40},{60,80}})));
  ThermalZone zon2(
    redeclare package Medium = Medium,
    energyDynamics=Modelica.Fluid.Types.Dynamics.FixedInitial,
    zoneName="Zone 2",
    fmuName="bld1.fmu")            "Thermal zone"
    annotation (Placement(transformation(extent={{20,-20},{60,20}})));
  ThermalZone zon3(
    zoneName="Zone 1",
    redeclare package Medium = Medium,
    energyDynamics=Modelica.Fluid.Types.Dynamics.FixedInitial,
    fmuName="bld2.fmu")            "Thermal zone"
    annotation (Placement(transformation(extent={{20,-84},{60,-44}})));
  Modelica.Blocks.Sources.Constant qConGai_flow(k=0) "Convective heat gain"
    annotation (Placement(transformation(extent={{-80,0},{-60,20}})));
  Modelica.Blocks.Sources.Constant qRadGai_flow(k=0) "Radiative heat gain"
    annotation (Placement(transformation(extent={{-80,30},{-60,50}})));
  Modelica.Blocks.Routing.Multiplex3 multiplex3_1
    annotation (Placement(transformation(extent={{-40,0},{-20,20}})));
  Modelica.Blocks.Sources.Constant qLatGai_flow(k=0) "Latent heat gain"
    annotation (Placement(transformation(extent={{-80,-30},{-60,-10}})));
equation
  connect(qRadGai_flow.y,multiplex3_1. u1[1])  annotation (Line(
      points={{-59,40},{-52,40},{-52,17},{-42,17}},
      color={0,0,127},
      smooth=Smooth.None));
  connect(qConGai_flow.y,multiplex3_1. u2[1]) annotation (Line(
      points={{-59,10},{-42,10}},
      color={0,0,127},
      smooth=Smooth.None));
  connect(multiplex3_1.u3[1],qLatGai_flow. y) annotation (Line(points={{-42,3},
          {-52,3},{-52,-20},{-59,-20}}, color={0,0,127}));
  connect(multiplex3_1.y, zon2.qGai_flow)
    annotation (Line(points={{-19,10},{18,10}}, color={0,0,127}));
  connect(multiplex3_1.y, zon1.qGai_flow) annotation (Line(points={{-19,10},{0,
          10},{0,70},{18,70}}, color={0,0,127}));
  connect(multiplex3_1.y, zon3.qGai_flow) annotation (Line(points={{-19,10},{0,
          10},{0,-54},{18,-54}}, color={0,0,127}));
  annotation (Documentation(info="<html>
<p>
Simple test case for two buildings with three thermal zones.
</p>
</html>", revisions="<html>
<ul><li>
February 14, 2018, by Michael Wetter:<br/>
First implementation.
</li>
</ul>
</html>"),
 __Dymola_Commands(file="modelica://Buildings/Resources/Scripts/Dymola/ThermalZones/EnergyPlus/Validation/ThreeZonesTwoBuildings.mos"
        "Simulate and plot"),
 experiment(
      StopTime=86400,
      Tolerance=1e-06));
end ThreeZonesTwoBuildings;
