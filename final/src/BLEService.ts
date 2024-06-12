import { BleClient, numbersToDataView } from '@capacitor-community/bluetooth-le';

const SENSOR_SERVICE_UUID = '4fafc201-1fb5-459e-8fcc-c5c9c331914b';
const SENSOR_CHARACTERISTIC_UUID = 'beb5483e-36e1-4688-b7f5-ea07361b26a8';

export async function main(): Promise<void> {
  try {
    await BleClient.initialize();

    const device = await BleClient.requestDevice({
      services: [SENSOR_SERVICE_UUID],
    });

    // connect to device, the onDisconnect callback is optional
    await BleClient.connect(device.deviceId, (deviceId) => onDisconnect(deviceId));
    console.log('connected to device', device);

    const sensorData = await BleClient.read(device.deviceId, SENSOR_SERVICE_UUID, SENSOR_CHARACTERISTIC_UUID);
    console.log('initial sensor data', parseSensorData(sensorData));

    await BleClient.startNotifications(
      device.deviceId,
      SENSOR_SERVICE_UUID,
      SENSOR_CHARACTERISTIC_UUID,
      (value) => {
        console.log('current sensor data', parseSensorData(value));
      }
    );

    // disconnect after 10 sec
    setTimeout(async () => {
      await BleClient.stopNotifications(device.deviceId, SENSOR_SERVICE_UUID, SENSOR_CHARACTERISTIC_UUID);
      await BleClient.disconnect(device.deviceId);
      console.log('disconnected from device', device);
    }, 10000);
  } catch (error) {
    console.error(error);
  }
}

function onDisconnect(deviceId: string): void {
  console.log(`device ${deviceId} disconnected`);
}

function parseSensorData(value: DataView): { temperature: number; lux: number } {
  // Assuming the data is formatted as two float32 values
  const temperature = value.getFloat32(0, true);
  const lux = value.getFloat32(4, true);
  return { temperature, lux };
}
