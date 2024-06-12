import React, { useState, useEffect } from 'react';
import { IonApp, IonHeader, IonToolbar, IonTitle, IonButton, IonText, IonAlert } from '@ionic/react';
import { BleClient } from '@capacitor-community/bluetooth-le';

const SENSOR_SERVICE_UUID = '4fafc201-1fb5-459e-8fcc-c5c9c331914b';
const SENSOR_CHARACTERISTIC_UUID = 'beb5483e-36e1-4688-b7f5-ea07361b26a8';

const App: React.FC = () => {
  const [sensorData, setSensorData] = useState<{ temperature: number; lux: number } | null>(null);
  const [error, setError] = useState<string | null>(null);
  const [alertOpen, setAlertOpen] = useState(false);

  useEffect(() => {
    BleClient.initialize();
  }, []);

  const handleConnect = async () => {
    try {
      setError(null); // Reset error state
      setAlertOpen(true); // Show alert with instructions
      const device = await BleClient.requestDevice({
        services: [SENSOR_SERVICE_UUID],
      });

      await BleClient.connect(device.deviceId, (deviceId) => onDisconnect(deviceId));
      console.log('Connected to device', device);

      await BleClient.startNotifications(
        device.deviceId,
        SENSOR_SERVICE_UUID,
        SENSOR_CHARACTERISTIC_UUID,
        (value) => {
          setSensorData(parseSensorData(value));
        }
      );
      setAlertOpen(false); // Close alert after successful connection
    } catch (error) {
      console.error('Failed to connect to device', error);
      setError('Failed to connect to device: ' + error.message);
      setAlertOpen(false); // Ensure alert is closed if connection fails
    }
  };

  const onDisconnect = (deviceId: string) => {
    console.log(`Device ${deviceId} disconnected`);
    setSensorData(null); // Clear sensor data on disconnect
  };

  const parseSensorData = (value: DataView): { temperature: number; lux: number } => {
    const str = new TextDecoder().decode(value.buffer);
    const [temperature, lux] = str.split(',').map(Number);
    return { temperature, lux };
  };

  return (
    <IonApp>
      <IonHeader>
        <IonToolbar style={{ display: 'flex', justifyContent: 'center', alignItems: 'center', flexDirection: 'column' }}>
          <IonTitle style={{ textAlign: 'center', marginBottom: '10px' }}>
            {sensorData
              ? `Temperature: ${sensorData.temperature.toFixed(2)} °C, Light Intensity: ${sensorData.lux.toFixed(2)} lux`
              : 'No data available'}
          </IonTitle>
          <IonButton style={{ backgroundColor: '#FF5722', color: '#FFFFFF' }} onClick={handleConnect}>
            Connect to Sensor
          </IonButton>
        </IonToolbar>
      </IonHeader>
      {error && (
        <IonText color="danger" style={{ fontSize: '20px', textAlign: 'center', marginTop: '20px' }}>
          {error}
        </IonText>
      )}
      <IonAlert
        isOpen={alertOpen}
        onDidDismiss={() => setAlertOpen(false)}
        header={'Connecting to Sensor'}
        message={'Please select your sensor device from the Bluetooth pairing dialog.'}
        buttons={['OK']}
      />
    </IonApp>
  );
};

export default App;
