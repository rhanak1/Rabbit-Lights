import { useState, useEffect } from 'react'
import { Form, InputNumber, Button, message, Layout, Space } from 'antd'
import './App.css'

const { Header, Content, Footer } = Layout

function App() {
  const [form] = Form.useForm()
  const [loading, setLoading] = useState(false)
  const [deviceState, setDeviceState] = useState('idle')
  const [raceStatus, setRaceStatus] = useState({ distanceLeft: 0, position: 0, runnerPosition: 0 })

  useEffect(() => {
    const loadInitialStatus = async () => {
      try {
        const res = await fetch('http://10.21.245.247/status');

        if (!res.ok) {
          throw new Error();
        }

        const data = await res.json();

        setDeviceState(data.state);

        setRaceStatus({
          distanceLeft: data.distanceLeft,
          position: data.position,
          runnerPosition: data.runnerPosition,
        });
      } catch {
        setDeviceState('offline');
      }
    };

    loadInitialStatus();
  }, []);

  useEffect(() => {
    let reconnectTimer: number;

    const connect = () => {
      const ws = new WebSocket(
        'ws://10.21.245.247:81'
      );

      ws.onopen = () => {
        console.log('WebSocket connected');
      };

      ws.onmessage = (event) => {
        const data = JSON.parse(event.data);

        if (data.type !== 'status') {
          return;
        }

        setDeviceState(data.state);

        setRaceStatus({
          distanceLeft: data.distanceLeft,
          position: data.position,
          runnerPosition: data.runnerPosition,
        });
      };

      ws.onclose = () => {
        console.log('WebSocket disconnected');

        setDeviceState('offline');

        reconnectTimer = window.setTimeout(
          connect,
          2000
        );
      };

      ws.onerror = () => {
        ws.close();
      };

      return ws;
    };

    const socket = connect();

    return () => {
      clearTimeout(reconnectTimer);
      socket.close();
    };
  }, []);

  const onFinish = async (values: { pace_min: number; pace_sec: number; distance: number }) => {
    try {
      setLoading(true)

      const payload = {
        pace: values.pace_min * 60 + values.pace_sec,
        distance: values.distance,
      }

      const res = await fetch('http://10.21.245.247/set', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify(payload),
      })

      if (!res.ok) {
        throw new Error(`Request failed: ${res.status}`)
      }

      message.success('Values sent to ESP32')
      form.resetFields()
    } catch (err) {
      message.error('Failed to send values')
      console.error(err)
    } finally {
      setLoading(false)
    }
  }

  return (
    <Layout>
      <Header style={{height:128}}>
        <h1 style={{ color: 'white', textAlign: 'left' }}>Rabbit Lights</h1>
      </Header>
      <Content>
        <section id="center">
          <div>
            <h1 style={{color: 'black'}}>Set Lights</h1>

            <Form form={form} onFinish={onFinish} layout="vertical" disabled={deviceState === 'running'}>
              <Space.Compact block>
                <Form.Item
                  label="Minutes"
                  name="pace_min"
                  rules={[{ required: true, message: 'Enter a pace value' }]}
                >
                  <InputNumber style={{ width: '100%' }} />
                </Form.Item>
                <Form.Item
                  label="Seconds"
                  name="pace_sec"
                  rules={[{ required: true, message: 'Enter a pace value' }]}
                >
                  <InputNumber style={{ width: '100%' }} />
                </Form.Item>
              </Space.Compact>

              <Form.Item
                label="Distance (meters)"
                name="distance"
                rules={[{ required: true, message: 'Enter a distance value' }]}
              >
                <InputNumber style={{ width: '100%' }} />
              </Form.Item>

              <Form.Item>
                <Button type="primary" htmlType="submit" loading={loading}>
                  Submit
                </Button>
              </Form.Item>
            </Form>
            <p>Distance Left: {raceStatus.distanceLeft}</p>
            <p>Position: {raceStatus.position}</p>
            <p>Runner Position: {raceStatus.runnerPosition}</p>
          </div>
        </section>
      </Content>
      <Footer>
        <p style={{ textAlign: 'left' }}>Device State: {deviceState}</p>
      </Footer>
    </Layout>
  )
}

export default App